#if ZE_WITH_EDITOR

#include "gfx/effect/EffectCompiler.h"
#include "assetdatacache/AssetDatacache.h"

namespace ze::gfx
{

EffectCompilerResult compile_effect(const ShaderFormat& in_shader_format,
	const std::string& in_permutation_name,
	const robin_hood::unordered_map<ShaderStageFlagBits, std::string>& in_source_map, 
	const std::vector<std::pair<std::string, std::string>>& in_options,
	const bool in_cache)
{
	logger::info("Compiling effect permutation {} ({})", in_permutation_name, std::to_string(in_shader_format));
	
	EffectCompilerResult result;

	/**
	 * Compile each stage asynchrnously and wait for them
	 */
	struct Stage
	{
		ShaderStageFlagBits stage;
		ShaderCompilerOutput output;

		Stage(const ShaderStageFlagBits in_stage,
			ShaderCompilerOutput&& in_output) : stage(in_stage), output(std::move(in_output)) {}
		Stage(Stage&& in_other) noexcept : stage(std::move(in_other.stage)), output(std::move(in_other.output)) {}

		Stage(const Stage&) = delete;
		void operator=(const Stage&) = delete;
	};

	/** Compile all stages */
	std::vector<Stage> stages;
	stages.reserve(20);
	for(const auto& [stage, source] : in_source_map)
	{
		std::string entry_point;
		switch(stage)
		{
		default:
			entry_point = "main";
			break;
		case ShaderStageFlagBits::Vertex:
			entry_point = "vertex";
			break;
		case ShaderStageFlagBits::Fragment:
			entry_point = "fragment";
			break;
		case ShaderStageFlagBits::Geometry:
			entry_point = "geometry";
			break;
		case ShaderStageFlagBits::Compute:
			entry_point = "compute";
			break;
		case ShaderStageFlagBits::TesselationControl:
			entry_point = "tess_control";
			break;
		case ShaderStageFlagBits::TesselationEvaluation:
			entry_point = "tess_eval";
			break;
		}

		/** TODO: Async, I can't get this to work asynchrnously but I wonder if it will really benifit from that */
		stages.emplace_back(stage, compile_shader(stage,
			in_permutation_name + "_" + entry_point,
			source,
			entry_point,
			in_shader_format,
			true));
	}

	result.succeed = true;
	
	for(auto& stage : stages)
	{
		const auto& output = stage.output;

		/** If one stage failed to compile, do not reset the variable later */
		if(result.succeed)
			result.succeed = output.success;
		result.stages.emplace_back(stage.stage, output.bytecode, output.reflection_data, output.err_msg);
	}

	/** Cache to assetdatacache */
	if(result.succeed && in_cache)
	{
		for(const auto& stage : stages)
		{
			const std::string key = "Effect_" + in_permutation_name + "_" + std::to_string(stage.stage) + "_" + std::to_string(in_shader_format);
			assetdatacache::cache("EffectCompiler", key, {'a', 'b', 'c'});
		}
	}

	return result;
}

}

#endif