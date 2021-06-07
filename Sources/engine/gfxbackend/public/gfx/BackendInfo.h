#pragma once

#include "EngineCore.h"
#include "ShaderFormat.h"

namespace ze::gfx
{
/**
 * Enumerate all shader models
 * A shader model is a set of shaders-related features
 * This enum uses the D3D shader model format as it is the most common one found
 */
enum class BackendShaderModel
{
	/** Implements features like tesselation, waves ops */
	ShaderModel6_0,

	/** Implements feature like raytracing, mesh/amplifications shaders */
	ShaderModel6_5,
};

enum class BackendShaderLanguage
{
	SPIRV,
	DXIL
};
	
/**
 * Basic informations about a graphics backend
 * Loaded from Config/Backends TOML files
 */
struct BackendInfo
{
	std::string name;
	std::string module_name;

	/** (Intermediate) shader language used by this backend */
	BackendShaderLanguage shader_language;
	
	/** Supported shader models by this backend, does not depend on the running platform ! */
	std::vector<BackendShaderModel> shader_models;

	BackendInfo(const std::string& in_name,
		const std::string& in_module_name,
		const BackendShaderLanguage& in_shader_language,
		const std::vector<BackendShaderModel>& in_shader_models)
		: name(in_name), module_name(in_module_name), shader_language(in_shader_language),
		shader_models(in_shader_models) {}
	
	ShaderFormat get_shader_format(BackendShaderModel in_shader_model) const
	{
		switch(in_shader_model)
		{
		default:
		case BackendShaderModel::ShaderModel6_0:
			if(shader_language == BackendShaderLanguage::SPIRV)
				return ShaderFormat::SPIRV_SM_6_0;
			else
				return ShaderFormat::DXIL_SM_6_0;
		case BackendShaderModel::ShaderModel6_5:
			if(shader_language == BackendShaderLanguage::SPIRV)
				return ShaderFormat::SPIRV_SM_6_5;
			else
				return ShaderFormat::DXIL_SM_6_5;
		}
	}
};
}