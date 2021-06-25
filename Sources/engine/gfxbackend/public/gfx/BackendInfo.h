#pragma once

#include "EngineCore.h"
#include "ShaderFormat.h"

namespace ze::gfx
{

/**
 * Basic informations about a graphics backend
 * Loaded from Config/Backends TOML files
 */
struct BackendInfo
{
	std::string name;
	std::string module_name;

	/** (Intermediate) shader language used by this backend */
	ShaderLanguage shader_language;
	
	/** Supported shader models by this backend, does not depend on the running platform ! */
	std::vector<ShaderModel> shader_models;

	BackendInfo(const std::string& in_name,
		const std::string& in_module_name,
		const ShaderLanguage& in_shader_language,
		const std::vector<ShaderModel>& in_shader_models)
		: name(in_name), module_name(in_module_name), shader_language(in_shader_language),
		shader_models(in_shader_models) {}
	
	ShaderFormat get_shader_format(ShaderModel in_shader_model) const
	{
		return { shader_language, in_shader_model };
	}
};
}