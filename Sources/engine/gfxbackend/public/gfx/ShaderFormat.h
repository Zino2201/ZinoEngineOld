#pragma once

#include "EngineCore.h"

namespace ze::gfx
{

/**
 * Enumerate all shader models
 * A shader model is a set of shaders-related features
 * This enum uses the D3D shader model format as it is the most common one found
 */
enum class ShaderModel
{
	/** Implements features like tesselation, waves ops */
	ShaderModel6_0,

	/** Implements feature like raytracing, mesh/amplifications shaders */
	ShaderModel6_5,
};

enum class ShaderLanguage
{
	SPIRV,
	DXIL
};
	
/**
 * Represents a format a shader can be compiled into
 */
struct ShaderFormat
{
	ShaderLanguage language;
	ShaderModel model;

	ShaderFormat(const ShaderLanguage& in_language, const ShaderModel& in_model)
		: language(in_language), model(in_model) {}
};
	
}

namespace std
{

inline std::string to_string(const ze::gfx::ShaderLanguage& in_shader_language)
{
	switch(in_shader_language)
	{
	default:
		return "";
	case ze::gfx::ShaderLanguage::SPIRV:
		return "SPIRV";
	case ze::gfx::ShaderLanguage::DXIL:
		return "DXIL";
	}
}

inline std::string to_string(const ze::gfx::ShaderModel& in_shader_model)
{
	switch(in_shader_model)
	{
	default:
		return "";
	case ze::gfx::ShaderModel::ShaderModel6_0:
		return "ShaderModel6_0";
	case ze::gfx::ShaderModel::ShaderModel6_5:
		return "ShaderModel6_5";
	}
}
	
inline std::string to_string(const ze::gfx::ShaderFormat& in_shader_format)
{
	return std::to_string(in_shader_format.language) + "_" + std::to_string(in_shader_format.model);
}
	
}