#include "Module/Module.h"
#define SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS 1
#include "Shader/ShaderCompiler.h"
#include <spirv_glsl.hpp>
#include "ShaderConductor/ShaderConductor.hpp"
#include <array>
#include "ZEFS/Utils.h"
#include "ZEFS/Paths.h"
#include "ZEFS/ZEFS.h"

using namespace ZE;

static constexpr std::array<const char*, 1> GIncludeDirs =
{
	"Shaders"
};

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
		std::string ShaderData = ZE::FileSystem::ReadFileToString(InShaderFilename);
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
		SourceDesc.loadIncludeCallback = 
			[](const char* InIncludeName) -> ShaderConductor::Blob*
			{	
				std::string Filename = std::filesystem::path(InIncludeName).filename().string();
				std::string Path = InIncludeName;

				if(!ZE::FileSystem::Exists(InIncludeName))
				{
					bool bHasFoundFile = false;

					for (const auto& IncludeDir : GIncludeDirs)
					{
						Path = IncludeDir;
						Path += "/";
						Path += Filename;
						
						if(ZE::FileSystem::Exists(Path))
						{
							bHasFoundFile = true;
							break;
						}
					}

					if(!bHasFoundFile)
						return nullptr;
				}

				std::vector<uint8_t> Array = ZE::FileSystem::ReadFileToVector(Path);

				ShaderConductor::Blob* Blob = ShaderConductor::CreateBlob(Array.data(),
					static_cast<uint32_t>(Array.size()));

				return Blob;
			};

		ShaderConductor::Compiler::Options Options;
		Options.disableOptimizations = bInShouldOptimize;
#ifdef ZE_DEBUG
		Options.enableDebugInfo = true;
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
			ZE::Logger::Error("Failed to compile shader {}: {}",
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

		spirv_cross::Compiler Compiler(InSpv);

		spirv_cross::ShaderResources Resources = Compiler.get_shader_resources();
		for (auto& UniformBuffer : Resources.uniform_buffers)
		{
			const auto& Type = Compiler.get_type(UniformBuffer.base_type_id);

			uint32_t Set = Compiler.get_decoration(UniformBuffer.id, spv::DecorationDescriptorSet);
			uint32_t Binding = Compiler.get_decoration(UniformBuffer.id, spv::DecorationBinding);
			std::string Name = Compiler.get_name(UniformBuffer.id);
			if (Name.empty())
				Name = Compiler.get_name(UniformBuffer.base_type_id);

			uint64_t Size = Compiler.get_declared_struct_size(Type);

			std::vector<SShaderParameterMember> Members; /*=
				ParseStruct(Compiler.get(), Type);*/

			Output.ParameterMap.AddParameter(Name.c_str(), 
				{
					Name,
					EShaderParameterType::UniformBuffer,
					Set,
					Binding,
					Size,
					1,
					Members
				});
		}

		for (auto& Texture : Resources.separate_images)
		{
			uint32_t Set = Compiler.get_decoration(Texture.id, spv::DecorationDescriptorSet);
			uint32_t Binding = Compiler.get_decoration(Texture.id, spv::DecorationBinding);
			std::string Name = Compiler.get_name(Texture.id);

			Output.ParameterMap.AddParameter(Name.c_str(), 
				SShaderParameter(
					Name,
					EShaderParameterType::Texture,
					Set,
					Binding,
					1));
		}

		for (auto& Sampler : Resources.separate_samplers)
		{
			uint32_t Set = Compiler.get_decoration(Sampler.id, spv::DecorationDescriptorSet);
			uint32_t Binding = Compiler.get_decoration(Sampler.id, spv::DecorationBinding);
			std::string Name = Compiler.get_name(Sampler.id);

			Output.ParameterMap.AddParameter(Name.c_str(),
				SShaderParameter(
					Name,
					EShaderParameterType::Sampler,
					Set,
					Binding,
					1));
		}

		return Output;
	}
};

class CVulkanShaderCompilerModule : public ZE::Module::CModule
{
public:
	CVulkanShaderCompilerModule()
	{
		CGlobalShaderCompiler::Get()
			.Register<CVulkanShaderCompiler>(EShaderCompilerTarget::VulkanSpirV);
	}
};

DEFINE_MODULE(CVulkanShaderCompilerModule, VulkanShaderCompiler);