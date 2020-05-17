#include "Module/Module.h"
#include "Shader/ShaderCompiler.h"
#include "IO/IOUtils.h"
#include <spirv_glsl.hpp>
#include "ShaderConductor/ShaderConductor.hpp"
#include <array>

using namespace ZE;

DECLARE_LOG_CATEGORY(VulkanShaderCompiler);

/**
 * Vulkan shader compiler
 */
class CVulkanShaderCompiler : public IShaderCompiler
{
public:
	CVulkanShaderCompiler(const EShaderCompilerTarget& InTarget) : IShaderCompiler(InTarget) {}

	SShaderCompilerOutput CompileShader(
		const EShaderStage& InStage,
		const std::string_view& InShaderFilename,
		const std::string_view& InEntryPoint,
		const EShaderCompilerTarget& InTargetFormat,
		const bool& bInShouldOptimize) override
	{
		SShaderCompilerOutput Output;

		/**
		 * Read shader data
		 */
		std::string ShaderData = IOUtils::ReadTextFile(InShaderFilename);
		if(ShaderData.empty())
		{
			Output.ErrMsg = "Can't open shader file";
			return Output;
		}

		ShaderConductor::ShaderStage Stage = ShaderConductor::ShaderStage::VertexShader;
		switch(InStage)
		{
		case EShaderStage::Fragment:
			Stage = ShaderConductor::ShaderStage::PixelShader;
			break;
		default:
			break;
		};

		ShaderConductor::Compiler::SourceDesc SourceDesc;
		SourceDesc.source = ShaderData.c_str();
		SourceDesc.fileName = InShaderFilename.data();
		SourceDesc.entryPoint = InEntryPoint.data();
		SourceDesc.stage = Stage;
		SourceDesc.numDefines = 0;
		SourceDesc.defines = nullptr;

		ShaderConductor::Compiler::Options Options;
		Options.disableOptimizations = bInShouldOptimize;
#ifdef _DEBUG
		Options.optimizationLevel = 1;
#else
		Options.optimizationLevel = 3;
#endif

		std::array<ShaderConductor::Compiler::TargetDesc, 1> Targets = 
		{
			{ ShaderConductor::ShadingLanguage::SpirV, "1_0", false }
		};

		ShaderConductor::Compiler::ResultDesc Result;
		ShaderConductor::Compiler::Compile(SourceDesc, Options, 
			Targets.data(), static_cast<uint32_t>(Targets.size()), 
			&Result);

		if(Result.hasError)
		{
			std::string_view ErrMsg(
				reinterpret_cast<const char*>(Result.errorWarningMsg->Data()), 
				Result.errorWarningMsg->Size());
			Output.ErrMsg = ErrMsg;
			LOG(ELogSeverity::Error, VulkanShaderCompiler, "Failed to compile shader %s: %s",
				InShaderFilename.data(), Output.ErrMsg.c_str());
			return Output;
		}

		uint32_t* Spv = reinterpret_cast<uint32_t*>(const_cast<void*>(Result.target->Data()));

		Output.Bytecode = std::vector<uint32_t>(
			Spv,
			Spv + (Result.target->Size() / sizeof(uint32_t)));
		Output.bSucceed = true;
		Output.ReflectionData = GetReflectionData(Output.Bytecode);

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

class CVulkanShaderCompilerModule : public CModule
{
public:
	void Initialize() override
	{
		CGlobalShaderCompiler::Get()
			.Register<CVulkanShaderCompiler>(EShaderCompilerTarget::VulkanSpirV);
	}
};

DEFINE_MODULE(CVulkanShaderCompilerModule, "VulkanShaderCompiler");