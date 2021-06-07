#include "gfx/BackendManager.h"

#include "gfx/Backend.h"
#include "zefs/FileStream.h"
#include "zefs/ZEFS.h"
#include "toml++/toml.h"
#include "module/ModuleManager.h"
#include "gfx/BackendModule.h"

namespace ze::gfx
{

std::vector<BackendInfo> backends;
Backend* running_backend = nullptr;

BackendShaderLanguage get_shader_language_from_string(const std::string& in_str)
{
	if(in_str == "SPIR-V")
		return BackendShaderLanguage::SPIRV;

	if(in_str == "DXIL")
		return BackendShaderLanguage::DXIL;
		

	ZE_CHECKF(true, "Invalid shader language {}", in_str);
	
	return BackendShaderLanguage::SPIRV;
}

BackendShaderModel get_shader_model_from_string(const std::string& in_str)
{
	if(in_str == "ShaderModel6_0")
		return BackendShaderModel::ShaderModel6_0;

	if(in_str == "ShaderModel6_5")
		return BackendShaderModel::ShaderModel6_5;
		

	ZE_CHECKF(true, "Invalid shader model {}", in_str);
	
	return BackendShaderModel::ShaderModel6_0;
}
	
void parse_backend_file(const std::filesystem::path& in_file)
{
	/** Don't add another backend if we already have a selected backend */
	if(running_backend)
		return;
	
	filesystem::FileIStream stream(in_file);
	auto root = toml::parse(stream, in_file.stem().string());
	for(const auto& backend : *root["Backend"].as_table())
	{
		auto backend_table = *backend.second.as_table();

		/** Every parameters is required */
		
		auto name = backend_table["Name"].as_string();
		if(!name)
		{
			logger::error("Invalid backend name for backend {} (file {})",
				backend.first,
				in_file.string());
			continue;
		}

		auto module = backend_table["Module"].as_string();
		if(!module)
		{
			logger::error("Invalid backend module for backend {} (file {})",
				backend.first,
				in_file.string());
			continue;
		}

		auto shader_language = backend_table["ShaderLanguage"].as_string();
		if(!shader_language)
		{
			logger::error("Invalid backend shader language for backend {} (file {})",
				backend.first,
				in_file.string());
			continue;
		}
		
		std::vector<BackendShaderModel> supported_shader_models;
		const auto shader_models = backend_table["ShaderModels"].as_array();
		if(!shader_models)
		{
			logger::error("Invalid backend shader models for backend {} (file {})",
				backend.first,
				in_file.string());
			continue;
		}

		for(const auto& model : *shader_models)
			supported_shader_models.emplace_back(get_shader_model_from_string(model.as_string()->get()));
		
		backends.emplace_back(name->get(), 
			module->get(),
			get_shader_language_from_string(shader_language->get()),
			supported_shader_models);

		logger::info("Registered backend {}", name->get());
	}
}
	
void find_backends()
{
	logger::info("Searching for backends in Config/Backends...");

	filesystem::iterate_directories("Config/Backends", 
		[&](const filesystem::DirectoryEntry& entry)
		{
			parse_backend_file("Config/Backends" / entry.path);
		});
}

const std::vector<BackendInfo>& get_backends()
{
	return backends;
}

bool create_backend(const BackendInfo* in_backend, const BackendShaderModel in_requested_shader_model)
{
	/** Try load the backend's module */
	if (module::Module* module = module::load_module(in_backend->module_name))
	{
		/** Try instancing the backend */
		BackendModule* backend_module = static_cast<BackendModule*>(module);
		Backend* backend = backend_module->create_backend(in_backend, in_requested_shader_model);
		if(!backend)
		{
			logger::error("Failed to create graphics backend {}", in_backend->name);
			return false;
		}

		running_backend = backend;
		
		logger::info("Using graphics backend {}", in_backend->name);

		return true;
	}

	logger::error("Failed to load graphics backend module {}", in_backend->name);
	
	return false;
}
	
Backend* get_running_backend()
{
	return running_backend;
}

void destroy_running_backend()
{
	if(running_backend)
		delete running_backend;
}
	
}