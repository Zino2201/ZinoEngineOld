#include "Module/Module.h"
#include "Shader/ShaderCompiler.h"
#include <shaderc/shaderc.hpp>
#include "IO/IOUtils.h"
#include <spirv_glsl.hpp>

using namespace ZE;

DECLARE_LOG_CATEGORY(VulkanShaderCompiler);

DEFINE_MODULE(CDefaultModule, "VulkanShaderCompiler");

/**
 * Vulkan shader compiler
 */
class CVulkanShaderCompiler : public IShaderCompiler
{
public:
	CVulkanShaderCompiler(const EShaderCompilerTarget& InTarget) : IShaderCompiler(InTarget) {}

	virtual SShaderCompilerOutput CompileShader(
		const EShaderStage& InStage,
		const std::string_view& InShaderFilename,
		const std::string_view& InEntryPoint,
		const EShaderCompilerTarget& InTargetFormat,
		const bool& bInShouldOptimize) override
	{
		/**
		 * Read shader data
		 */
		std::string ShaderData = IOUtils::ReadTextFile(InShaderFilename);

		/** Compile shader */
		shaderc::Compiler Compiler;
		shaderc::CompileOptions Options;

		shaderc_shader_kind Kind = shaderc_vertex_shader;

		/** Add define for shader stage type + set kind */
		// TODO: Move this to shader compiler
		switch (InStage)
		{
		case EShaderStage::Vertex:
			Kind = shaderc_vertex_shader;
			Options.AddMacroDefinition("VERTEX_SHADER");
			break;
		case EShaderStage::Fragment:
			Kind = shaderc_fragment_shader;
			Options.AddMacroDefinition("FRAGMENT_SHADER");
			break;
		}

		/** Preprocess data */
		std::string PreprocessedShader;
		{
			shaderc::PreprocessedSourceCompilationResult Result =
				Compiler.PreprocessGlsl(ShaderData, Kind, InShaderFilename.data(), Options);

			if (Result.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				LOG(ELogSeverity::Error, VulkanShaderCompiler, "Failed to preprocess shader %s: %s",
					InShaderFilename.data(), Result.GetErrorMessage().c_str());
				return {};
			}
			else
			{
				PreprocessedShader = { Result.cbegin(), Result.cend() };
			}
		}

		/** Compile */
		SShaderCompilerOutput Output;
		{
			Options.SetTargetEnvironment(shaderc_target_env_vulkan, 0);

			/** If in debug mode, don't optimize */
#ifdef _DEBUG
			Options.SetOptimizationLevel(bInShouldOptimize ?
				shaderc_optimization_level_size : shaderc_optimization_level_zero);
#else
			Options.SetOptimizationLevel(bInShouldOptimize ?
				shaderc_optimization_level_performance : shaderc_optimization_level_zero);
#endif

			shaderc::SpvCompilationResult Result = Compiler.CompileGlslToSpv(
				PreprocessedShader.c_str(),
				Kind,
				InShaderFilename.data(),
				InEntryPoint.data(),
				Options);

			if (Result.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				LOG(ELogSeverity::Error, VulkanShaderCompiler, "Failed to compile shader %s: %s",
					InShaderFilename.data(), Result.GetErrorMessage().c_str());
				return {};
			}
			else
			{
				Output.bSucceed = true;
				Output.Bytecode = { Result.begin(), Result.end() };
				Output.ReflectionData = GetReflectionData(Output.Bytecode);
			}
		}

		return Output;
	}

	SShaderCompilerReflectionDataOutput GetReflectionData(const std::vector<uint32_t>& InSpv)
	{
		SShaderCompilerReflectionDataOutput Output;

		spirv_cross::Compiler Compiler(std::move(InSpv));

		spirv_cross::ShaderResources Resources = Compiler.get_shader_resources();
		for (auto& UniformBuffer : Resources.uniform_buffers)
		{
			const auto& Type = Compiler.get_type(UniformBuffer.base_type_id);

			unsigned Set = Compiler.get_decoration(UniformBuffer.id, spv::DecorationDescriptorSet);
			unsigned Binding = Compiler.get_decoration(UniformBuffer.id, spv::DecorationBinding);
			std::string Name = Compiler.get_name(UniformBuffer.id);
			if (Name.empty())
				Name = Compiler.get_name(UniformBuffer.base_type_id);

			uint64_t Size = Compiler.get_declared_struct_size(Type);

			std::vector<SShaderParameterMember> Members; /*=
				ParseStruct(Compiler.get(), Type);*/

			Output.ParameterMap.AddParameter(Name.c_str(), 
				{
					Name.c_str(),
					EShaderParameterType::UniformBuffer,
					Set,
					Binding,
					Size,
					1,
					Members
				});
		}

		return Output;
	}
};

DEFINE_SHADER_COMPILER(EShaderCompilerTarget::VulkanSpirV, CVulkanShaderCompiler);