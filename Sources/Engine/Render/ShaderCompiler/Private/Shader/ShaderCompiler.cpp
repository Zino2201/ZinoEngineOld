#include "Shader/ShaderCompiler.h"
#include "Module/Module.h"

namespace ZE
{

DEFINE_MODULE(CDefaultModule, "ShaderCompiler")

DECLARE_LOG_CATEGORY(ShaderCompiler);

IShaderCompiler::IShaderCompiler(const EShaderCompilerTarget& InTarget) : Target(InTarget) 
{
	ShaderCompilers.insert(std::make_pair(InTarget, std::unique_ptr<IShaderCompiler>(this)));
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
	auto& Result = ShaderCompilers.find(InTargetFormat);

	if(Result != ShaderCompilers.end())
	{
		LOG(ELogSeverity::Info, ShaderCompiler, "Compiling shader (%s) %s",
			InStage == EShaderStage::Vertex ? "Vertex" : "Fragment",
			InShaderFilename.data());

		// TODO: SET DEFINES

		return std::async(std::launch::async,
			[Result, InStage, InShaderFilename, InEntryPoint,
				InTargetFormat, bInShouldOptimize]
			{
				SShaderCompilerOutput Output = Result->second->CompileShader(
					InStage,
					InShaderFilename,
					InEntryPoint,
					InTargetFormat,
					bInShouldOptimize);

				if(Output.bSucceed)
					LOG(ELogSeverity::Info, ShaderCompiler, "Shader %s compiled!",
						InShaderFilename.data());
				else
					LOG(ELogSeverity::Error, ShaderCompiler, "Failed to compile shader %s!",
						InShaderFilename.data());

				return Output;
			});
	}
	else
	{
		LOG(ELogSeverity::Fatal, ShaderCompiler, 
			"Failed to compile shader %s: unsupported format",
			InShaderFilename.data());
		return {};
	}
}

} /** namespace ZE */

/*CShaderCompilerOutput CShaderCompiler::CompileShaderFromGLSL(*/
//	const EShaderStage& InStage,
//	const std::string_view& InShaderFilename,
//	const std::string_view& InEntryPoint,
//	const EShaderCompilerTargetFormat& InTargetFormat,
//	const bool& bInShouldOptimize)
//{
//	LOG(ELogSeverity::Info, ShaderCompiler, "Compiling GLSL %s shader %s", 
//		InStage == EShaderStage::Vertex ? "Vertex" : "Fragment",
//		InShaderFilename.data());
//
//	/**
//	 * Read shader data
//	 */
//	std::string ShaderData = IOUtils::ReadTextFile(InShaderFilename);
//
//	/** Compile shader */
//	shaderc::Compiler Compiler;
//	shaderc::CompileOptions Options;
//
//	shaderc_shader_kind Kind = shaderc_vertex_shader;
//
//	/** Add define for shader stage type + set kind */
//	switch(InStage)
//	{
//	case EShaderStage::Vertex:
//		Kind = shaderc_vertex_shader;
//		Options.AddMacroDefinition("VERTEX_SHADER");
//		break;
//	case EShaderStage::Fragment:
//		Kind = shaderc_fragment_shader;
//		Options.AddMacroDefinition("FRAGMENT_SHADER");
//		break;
//	}
//
//	/** Preprocess data */
//	std::string PreprocessedShader;
//	{
//		shaderc::PreprocessedSourceCompilationResult Result =
//			Compiler.PreprocessGlsl(ShaderData, Kind, InShaderFilename.data(), Options);
//
//		if (Result.GetCompilationStatus() != shaderc_compilation_status_success) 
//		{
//			LOG(ELogSeverity::Error, ShaderCompiler, "Failed to preprocess shader %s: %s",
//				InShaderFilename.data(), Result.GetErrorMessage().c_str());
//			return {};
//		}
//		else
//		{
//			PreprocessedShader = { Result.cbegin(), Result.cend() };
//		}
//	}
//
//	/** Compile */
//	SShaderCompilerOutput Output;
//	{
//		Options.SetTargetEnvironment(shaderc_target_env_vulkan, 0);
//
//		/** If in debug mode, don't optimize */
//#ifdef _DEBUG
//		Options.SetOptimizationLevel(bInShouldOptimize ?
//			shaderc_optimization_level_size : shaderc_optimization_level_zero);
//#else
//		Options.SetOptimizationLevel(bInShouldOptimize ? 
//			shaderc_optimization_level_performance : shaderc_optimization_level_zero);
//#endif
//
//		shaderc::SpvCompilationResult Result = Compiler.CompileGlslToSpv(
//			PreprocessedShader.c_str(),
//			Kind,
//			InShaderFilename.data(),
//			InEntryPoint.data(),
//			Options);
//
// 		if (Result.GetCompilationStatus() != shaderc_compilation_status_success)
//		{
//			LOG(ELogSeverity::Error, ShaderCompiler, "Failed to compile shader %s: %s",
//				InShaderFilename.data(), Result.GetErrorMessage().c_str());
//			return {};
//		}
//		else
//		{
//			Output.bSucceed = true;
//			Output.Bytecode = { Result.begin(), Result.end() };
//		}
//	}
//	
//	return Output;
//}