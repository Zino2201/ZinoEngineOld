#include "Shader/ShaderCompiler.h"
#include "Module/Module.h"
#include "Threading/JobSystem/Async.h"

namespace ZE
{

DEFINE_MODULE(ZE::Module::CDefaultModule, ShaderCompiler)

IShaderCompiler::IShaderCompiler(const EShaderCompilerTarget& InTarget) : Target(InTarget) 
{
	
}

CGlobalShaderCompiler::CGlobalShaderCompiler() = default;
CGlobalShaderCompiler::~CGlobalShaderCompiler() = default;

std::future<SShaderCompilerOutput> CGlobalShaderCompiler::CompileShader(
	const EShaderStage& InStage,
	const std::string_view& InShaderFilename, 
	const std::string_view& InEntryPoint,
	const EShaderCompilerTarget& InTargetFormat,
	const bool& bInShouldOptimize)
{
	auto Result = ShaderCompilers.find(InTargetFormat);

	if(Result != ShaderCompilers.end())
	{
		ZE::Logger::Info("Compiling shader ({}) {}",
			InStage == EShaderStage::Vertex ? "Vertex" : "Fragment",
			InShaderFilename.data());

		// TODO: SET DEFINES

		std::string Path;
		Path += "Shaders/";
		Path += InShaderFilename;

		return ZE::Async<SShaderCompilerOutput>(
			[Result, InStage, Path = std::move(Path), InEntryPoint,
				InTargetFormat, bInShouldOptimize](const ZE::JobSystem::SJob& InJob)
			{
				SShaderCompilerOutput Output = Result->second->CompileShader(
					InStage,
					Path.c_str(),
					InEntryPoint,
					InTargetFormat,
					bInShouldOptimize);

				if(Output.bSucceed)
					ZE::Logger::Verbose("Shader {} compiled!",
						Path.c_str());
				else
					ZE::Logger::Error("Failed to compile shader {} !",
						Path.c_str());

				return Output;
			});
	}
	else
	{
		ZE::Logger::Error("Failed to compile shader {}: unsupported format",
			InShaderFilename.data());
		return {};
	}
}

} /** namespace ZE */