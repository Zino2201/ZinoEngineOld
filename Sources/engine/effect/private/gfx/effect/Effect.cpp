#include "gfx/effect/Effect.h"
#if ZE_WITH_EDITOR
#include "gfx/BackendManager.h"
#include "zefs/FileStream.h"
#include "threading/jobsystem/Async.h"
#endif
#include <bit>

namespace ze::gfx
{

#if ZE_WITH_EDITOR
std::mutex permutation_lock;
#endif

Effect::Effect(const std::string& in_name,
	const EffectShaderSources& in_sources,
	const std::vector<EffectOption>& in_options) : name(in_name),
	sources(in_sources), options(in_options), permutation_count(1)
{	
	int32_t required_bits = 0;
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
		required_bits += option.bit_width;
	}
	
	ZE_CHECKF(required_bits < max_permutation_bits, "Effect has too many options !");
}

Effect::~Effect()
{
	for(const auto& [id, map] : permutations)
		destroy_permutation(id);
}

#if ZE_WITH_EDITOR
std::future<EffectCompilerResult> Effect::compile(const EffectPermutationId id, const ShaderFormat& in_format)
{
	{
		std::lock_guard<std::mutex> lock(permutation_lock);

		if(pending_compilation.contains(id))
			return {};

		pending_compilation.insert(id);
	}
	
	return jobsystem::async<EffectCompilerResult>(
		[&, id](const jobsystem::Job&)
		{
			EffectCompilerResult result;
			result = compile_effect(in_format,
				name + "_" + std::to_string(id.to_ullong()),
				sources,
				get_options_from_id(id));

			/** Apply changes to class */
			{
				std::lock_guard<std::mutex> lock(permutation_lock);
				pending_compilation.erase(id);

				if(result.succeed)
				{
					for(const auto& stage : result.stages)
					{
						auto bytecode_copy = stage.bytecode;
						permutations[id].shader_map[stage.stage] = Device::get().create_shader(
							ShaderCreateInfo(bytecode_copy)).second;	
					}
					
					permutations[id].pipeline_layout = std::move(result.pipeline_layout);
					permutations[id].status = EffectPermutationStatus::Available;
				}
				else
				{
					permutations[id].status = EffectPermutationStatus::CompileError;
				}
			}
			
			return result;
		});
}
#endif

void Effect::destroy_permutation(EffectPermutationId id)
{
	for(const auto& [stage, shader] : permutations[id].shader_map)
	{
		if(shader)
			Device::get().destroy_shader(shader);
	}
}

Effect::Permutation* Effect::get_permutation(const EffectPermutationId id)
{
#if ZE_WITH_EDITOR
	{
		std::lock_guard<std::mutex> lock(permutation_lock);
		auto it = permutations.find(id);

		if(it != permutations.end())
			return &it->second;

		if(pending_compilation.contains(id))
			return {};
	}
	
	/** If the effect doesn't exist compile it */
	compile(id, Backend::get().get_shader_format(get_current_shader_model()));
#else
	auto it = permutations.find(id);
	if(it != permutations.end())
		return &it->second;
	
	ze::logger::error("No effect {} permutation {} found !",
		name,
		id.to_string());
#endif

	return nullptr;
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

bool Effect::is_available(const EffectPermutationId id)
{
	return permutations[id].status == EffectPermutationStatus::Available;
}

}