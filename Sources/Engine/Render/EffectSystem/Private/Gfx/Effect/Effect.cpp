#include "Gfx/Effect/Effect.h"
#if ZE_WITH_EDITOR
#include "AssetDataCache/AssetDataCache.h"
#include "Shader/ShaderCompiler.h"
#include "Threading/JobSystem/Async.h"
#endif
#include <bit>

namespace ze::gfx
{

std::mutex permutation_lock;

Effect::Effect(const std::string& in_name,
	const EffectShaderSources& in_sources,
	const std::vector<EffectOption>& in_options) : name(in_name),
	sources(in_sources), options(in_options), permutation_count(1)
{	
#if ZE_FEATURE(DEVELOPMENT)
	int32_t required_bits = 0;
#endif
	size_t idx = 0;
	size_t id_idx = 0;
	/** Calculate permutation count */
	for(auto& option : options)
	{
		if(option.type == EffectOptionType::Bool)
			permutation_count *= 2;
		else
			permutation_count *= option.count;

		option.bit_width = std::bit_width(option.count);
		option.id_index = id_idx;
		id_idx += option.bit_width;
		name_to_option_idx.insert({ option.name, idx++ });
#if ZE_FEATURE(DEVELOPMENT)
		required_bits += option.bit_width;
#endif
	}
#if ZE_FEATURE(DEVELOPMENT)
	ZE_CHECKF(required_bits < max_permutation_bits, "Effect has too many options !");
#endif
}

Effect::~Effect()
{
	for(const auto& [id, map] : permutations)
		destroy_permutation(id);
}

#if ZE_WITH_EDITOR

void Effect::compile_permutation_stage(EffectPermutationId id, ShaderStageFlagBits stage)
{
	using namespace shaders;

	ShaderStage cl_stage = ShaderStage::Vertex;
	std::string cl_name = name + "_";
	std::string entrypoint = "vertex";
	switch(stage)
	{
	case ShaderStageFlagBits::Vertex:
		cl_name += "VS";
		break;
	case ShaderStageFlagBits::Fragment:
		cl_stage = ShaderStage::Fragment;
		cl_name += "FS";
		entrypoint = "fragment";
		break;
	}

	auto output = compile_shader(
		cl_stage,
		cl_name,
		sources[stage],
		entrypoint,
		ShaderCompilerTarget::VulkanSpirV,
		true);

	if(!output.success) 
	{
		ze::logger::error("Failed to compile effect {} permutation {} shader {}: {}", 
			name,
			id.to_string(),
			cl_name,
			output.err_msg);
		permutations[id][stage] = {};
		return;
	}

	std::lock_guard<std::mutex> lock(permutation_lock);
	permutations[id][stage] = RenderBackend::get().shader_create(ShaderCreateInfo(output.bytecode)).second;
		
	std::string key = name + "_" + get_stage_prefix(stage);
	assetdatacache::cache("Effect permutation stage compilation", key, 
		std::vector<uint8_t>(
			reinterpret_cast<uint8_t*>(output.bytecode.data()), 
			reinterpret_cast<uint8_t*>(output.bytecode.data() + (output.bytecode.size() * sizeof(uint32_t)))));
}

#endif

void Effect::destroy_permutation(EffectPermutationId id)
{
	for(const auto& [stage, shader] : permutations[id])
	{
		if(shader)
			RenderBackend::get().shader_destroy(shader);
	}
}

const EffectShaderMap Effect::get_permutation(EffectPermutationId id)
{
	auto it = permutations.find(id);
	if(it != permutations.end())
		return it->second;

#if ZE_WITH_EDITOR
	/** Before doing anything, checks if the permutation exists in the cache */
	std::vector<ShaderStageFlagBits> stage_to_compile;
	for(const auto& [stage, source] : sources)
	{
		std::string key = name + "_" + get_stage_prefix(stage);

		if(assetdatacache::has_key(key))
		{
			std::vector<uint8_t> bytecode = assetdatacache::get_sync(key);
			std::lock_guard<std::mutex> lock(permutation_lock);
			permutations[id][stage] = RenderBackend::get().shader_create(gfx::ShaderCreateInfo(
				std::span<uint32_t>(
					reinterpret_cast<uint32_t*>(bytecode.data()), 
					reinterpret_cast<uint32_t*>(bytecode.data() + (bytecode.size() / sizeof(uint32_t)))))).second;
		}
		else
		{
			stage_to_compile.emplace_back(stage);
		}
	}

	using namespace jobsystem;

	if(!stage_to_compile.empty())
	{
		pending_compilation.insert(id);
		const auto& main_job = create_job(JobType::Normal,
			[this, id, stage_to_compile](const Job& in_job) 
			{
				const auto& root_job = create_job(JobType::Normal, [](const Job& in_job){});

				for(const auto& stage : stage_to_compile)
				{
					const auto& child = create_child_job(JobType::Normal, root_job,
						[this, id, stage](const Job& in_job)
						{
							compile_permutation_stage(id, stage);
						});

					schedule(child);
				}

				schedule(root_job);
				wait(root_job);

				pending_compilation.erase(id);
			});

		schedule(main_job);
	}
#else
	ze::logger::error("No effect {} permutation {} found !",
		name,
		id.to_string());
#endif

	return {};
}

std::vector<std::pair<std::string, std::string>> Effect::get_options_from_id(EffectPermutationId id) const
{
	std::vector<std::pair<std::string, std::string>> opts;
	opts.reserve(options.size());

	size_t j = 0;
	for(size_t i = 0; i < options.size(); ++i)
	{
		if(options[i].type == EffectOptionType::Bool)
		{
			opts.emplace_back(options[i].name, id.test(j++) ? "1" : "0");
		}
		else
		{
			uint32_t num = 0;
			for(size_t k = 0; k < options[i].bit_width; ++k)
				num = (num << 1) | id[k];

			opts.emplace_back(options[i].name, std::to_string(num));
			j += options[i].bit_width;
		}
	}

	return opts;
}

EffectPermutationId Effect::get_permutation_id(const std::vector<std::pair<std::string, uint32_t>>& in_enabled_options) const
{	
	EffectPermutationId id;

	for(const auto& enabled_option : in_enabled_options)
	{
		auto it = name_to_option_idx.find(enabled_option.first);
		ZE_CHECK(it != name_to_option_idx.end());
		size_t idx = it->second;
		size_t id_idx = options[it->second].id_index;

		if(options[idx].type == EffectOptionType::Bool)
		{
			id[id_idx] = static_cast<bool>(enabled_option.second);
		}
		else
		{
			uint32_t num = enabled_option.second;
			ZE_CHECKF(std::bit_width(num) <= options[idx].bit_width, "Number for effect option {} is too big ! (too many bits)", enabled_option.first);
			for(size_t i = 0; i < options[idx].bit_width; ++i)
			{
				id[id_idx + i] = num & 1;
				num >>= 1;
			}
		}
	}

	return id;
}

bool Effect::is_available(EffectPermutationId id) const
{
#if ZE_WITH_EDITOR
	return permutations.contains(id) && !pending_compilation.contains(id);
#else
	return permutations.contains(id);
#endif
}

std::string Effect::get_stage_prefix(ShaderStageFlagBits stage) const
{
	switch(stage)
	{
	case ShaderStageFlagBits::Vertex:
		return "VS";
	case ShaderStageFlagBits::Fragment:
		return "FS";
	}
}

}