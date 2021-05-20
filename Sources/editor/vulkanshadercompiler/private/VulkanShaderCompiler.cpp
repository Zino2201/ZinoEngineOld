#include "module/Module.h"
#define SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS 1
#include "shader/ShaderCompiler.h"
#include <spirv_glsl.hpp>
#include "ShaderConductor/ShaderConductor.hpp"
#include <array>
#include "zefs/Utils.h"
#include "zefs/Paths.h"
#include "zefs/ZEFS.h"

using namespace ze::gfx::shaders;

static constexpr std::array<const char*, 1> include_dirs =
{
	"Shaders"
};

/**
 * Vulkan shader compiler
 */
class VulkanShaderCompiler : public ShaderCompiler
{
public:
	VulkanShaderCompiler(const ShaderCompilerTarget& in_target) : ShaderCompiler(in_target) {}

	ShaderCompilerOutput compile_shader(
		const ShaderStage& stage,
		const std::string_view& shader_name,
		const std::string_view& shader_source,
		const std::string_view& entry_point,
		const ShaderCompilerTarget& target,
		const bool& optimize) override
	{
		ShaderCompilerOutput output;

		ShaderConductor::ShaderStage scstage = ShaderConductor::ShaderStage::VertexShader;
		switch(stage)
		{
		case ShaderStage::Fragment:
			scstage = ShaderConductor::ShaderStage::PixelShader;
			break;
		default:
			break;
		};

		ShaderConductor::Compiler::SourceDesc source_desc;
		source_desc.source = shader_source.data();
		source_desc.fileName = shader_name.data();
		source_desc.entryPoint = entry_point.data();
		source_desc.stage = scstage;
		source_desc.numDefines = 0;
		source_desc.defines = nullptr;
		source_desc.loadIncludeCallback = 
			[](const char* include_name) -> ShaderConductor::Blob*
			{	
				std::string filename = std::filesystem::path(include_name).filename().string();
				std::string path = include_name;

				if(!ze::filesystem::exists(include_name))
				{
					bool has_found_file = false;

					for (const auto& include_dir : include_dirs)
					{
						path = include_dir;
						path += "/";
						path += include_name;
						
						if(ze::filesystem::exists(path))
						{
							has_found_file = true;
							break;
						}
					}

					if(!has_found_file)
						return nullptr;
				}

				std::vector<uint8_t> array = ze::filesystem::read_file_to_vector(path);

				ShaderConductor::Blob* blob = ShaderConductor::CreateBlob(array.data(),
					static_cast<uint32_t>(array.size()));

				return blob;
			};

		ShaderConductor::Compiler::Options options;
		options.disableOptimizations = optimize;
#ifdef ZE_DEBUG
		options.enableDebugInfo = true;
		options.optimizationLevel = 1;
#else
		options.optimizationLevel = 3;
#endif

		std::array<ShaderConductor::Compiler::TargetDesc, 1> targets = 
		{
			{ { ShaderConductor::ShadingLanguage::SpirV, "1_0", false } }
		};

		ShaderConductor::Compiler::ResultDesc result = {};
		ShaderConductor::Compiler::Compile(source_desc, options, 
			targets.data(), static_cast<uint32_t>(targets.size()), 
			&result);

		if(result.hasError)
		{
			std::string_view err_msg(
				reinterpret_cast<const char*>(result.errorWarningMsg->Data()), 
				result.errorWarningMsg->Size());
			output.err_msg = err_msg;
			ze::logger::error("Failed to compile shader {}: {}",
				shader_name.data(), output.err_msg.c_str());
			return output;
		}

		uint32_t* spv = reinterpret_cast<uint32_t*>(const_cast<void*>(result.target->Data()));
		output.bytecode = std::vector<uint32_t>(
			spv,
			spv + (result.target->Size() / sizeof(uint32_t)));
		output.success = true;
		output.reflection_data = get_reflection_data(output.bytecode);

		return output;
	}

	ShaderCompilerReflectionDataOutput get_reflection_data(const std::vector<uint32_t>& spv)
	{
		ShaderCompilerReflectionDataOutput output;

		spirv_cross::Compiler compiler(spv);

		spirv_cross::ShaderResources resources = compiler.get_shader_resources();
		for (auto& uniform_buffer : resources.uniform_buffers)
		{
			const auto& type = compiler.get_type(uniform_buffer.base_type_id);

			uint32_t set = compiler.get_decoration(uniform_buffer.id, spv::DecorationDescriptorSet);
			uint32_t binding = compiler.get_decoration(uniform_buffer.id, spv::DecorationBinding);
			std::string name = compiler.get_name(uniform_buffer.id);
			if (name.empty())
				name = compiler.get_name(uniform_buffer.base_type_id);

			uint64_t size = compiler.get_declared_struct_size(type);

			std::vector<ShaderParameterMember> members ;

			output.parameter_map.add_parameter(name.c_str(), 
				{
					name,
					ShaderParameterType::UniformBuffer,
					set,
					binding,
					size,
					1,
					members
				});
		}

		for (auto& texture : resources.separate_images)
		{
			uint32_t set = compiler.get_decoration(texture.id, spv::DecorationDescriptorSet);
			uint32_t binding = compiler.get_decoration(texture.id, spv::DecorationBinding);
			std::string name = compiler.get_name(texture.id);

			output.parameter_map.add_parameter(name.c_str(), 
				ShaderParameter(
					name,
					ShaderParameterType::Texture,
					set,
					binding,
					1));
		}

		for (auto& sampler : resources.separate_samplers)
		{
			uint32_t set = compiler.get_decoration(sampler.id, spv::DecorationDescriptorSet);
			uint32_t binding = compiler.get_decoration(sampler.id, spv::DecorationBinding);
			std::string name = compiler.get_name(sampler.id);

			output.parameter_map.add_parameter(name.c_str(),
				ShaderParameter(
					name,
					ShaderParameterType::Sampler,
					set,
					binding,
					1));
		}

		return output;
	}
};

class VULKANSHADERCOMPILER_API VulkanShaderCompilerModule : public ze::module::Module
{
public:
	VulkanShaderCompilerModule()
	{
		sc = register_shader_compiler(ShaderCompilerTarget::VulkanSpirV, new VulkanShaderCompiler(ShaderCompilerTarget::VulkanSpirV));
	}

	~VulkanShaderCompilerModule()
	{
		unregister_shader_compiler(sc);
	}
private:
	ShaderCompiler* sc;
};

ZE_DEFINE_MODULE(VulkanShaderCompilerModule, VulkanShaderCompiler);