#pragma once

#if ZE_WITH_EDITOR

#include <string>
#include "Platform.h"
#include "gfx/Gfx.h"
#include "shader/ShaderCompiler.h"
#include <robin_hood.h>

/**
 * Effect compiler
 */
namespace ze::gfx
{

/**
 * Result of the compilation of a single effect permutation
 */
struct EffectCompilerResult
{
	struct Stage
	{
		ShaderStageFlagBits stage;
		std::vector<uint32_t> bytecode;
		ShaderCompilerReflectionDataOutput reflection_data;
		std::string error_message;

		Stage(const ShaderStageFlagBits& in_stage,
			const std::vector<uint32_t>& in_bytecode,
			const ShaderCompilerReflectionDataOutput& in_reflection_data,
			const std::string& in_error_message) : stage(in_stage), bytecode(in_bytecode),
			reflection_data(in_reflection_data), error_message(in_error_message) {}
	};
	
	bool succeed;
	std::vector<Stage> stages;
	UniquePipelineLayout pipeline_layout;
	
	EffectCompilerResult() : succeed(false) {}
};
	
/**
 * Compile an single effect permutation from source (sync)
 * \param in_shader_format Shader format to compile to
 * \param in_permutation_name Permutation name
 * \param in_source_map Sources to compile per stage
 * \param in_options Options key, value pair
 * \param in_cache Should cache the result to the asset datacache ?
 * \return Compilation result
 */
EffectCompilerResult compile_effect(const ShaderFormat& in_shader_format,
	const std::string& in_permutation_name,
	const robin_hood::unordered_map<ShaderStageFlagBits, std::string>& in_source_map, 
	const std::vector<std::pair<std::string, std::string>>& in_options,
	const bool in_cache = true);
	
}

#endif