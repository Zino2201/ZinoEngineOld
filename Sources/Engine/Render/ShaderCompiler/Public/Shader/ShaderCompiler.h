#pragma once

#include "EngineCore.h"
#include "Render/RenderCore.h"
#include <future>
#include <string_view>

namespace ZE
{

/**
 * Target formats
 */
enum class EShaderCompilerTargetFormat
{
    SpirV,
};

/**
 * Reflection data output
 */
struct SShaderCompilerReflectionDataOutput
{
    SShaderParameterMap ParameterMap;
};

/**
 * Output of the shader compiler
 */
struct SShaderCompilerOutput
{
    bool bSucceed;
    std::vector<uint32_t> Bytecode;
    SShaderCompilerReflectionDataOutput ReflectionData;

    SShaderCompilerOutput() : bSucceed(false), Bytecode() {}
};

enum class EShaderCompilerTarget
{
    VulkanSpirV
};

/**
 * Shader compiler interface
 * Should be implemented for each target
 */
class SHADERCOMPILER_API IShaderCompiler
{
public:
    IShaderCompiler(const EShaderCompilerTarget& InTarget);

	virtual SShaderCompilerOutput CompileShader(
		const EShaderStage& InStage,
		const std::string_view& InShaderFilename,
		const std::string_view& InEntryPoint,
		const EShaderCompilerTarget& InTargetFormat,
		const bool& bInShouldOptimize) = 0;
protected:
    EShaderCompilerTarget Target;
};

#define DEFINE_SHADER_COMPILER(Target, Class) static Class* ShaderCompiler_##Class = new Class(Target)

static std::unordered_map<EShaderCompilerTarget, IShaderCompiler*> ShaderCompilers;

/**
 * Global shader compiler
 */
class SHADERCOMPILER_API CGlobalShaderCompiler
{
public:
    CGlobalShaderCompiler();
    ~CGlobalShaderCompiler();

    /**
     * Begin a shader compiling process
     */
    std::future<SShaderCompilerOutput> CompileShader(
        const EShaderStage& InStage,
        const std::string_view& InShaderFilename,
        const std::string_view& InEntryPoint,
        const EShaderCompilerTarget& InTargetFormat,
        const bool& bInShouldOptimize = true);
//private:
    /**
     * Compile a GLSL shader (use glslang)
     */
    //SShaderCompilerOutput CompileShaderFromGLSL(
    //    const EShaderStage& InStage,
    //    const std::string_view& InShaderFilename,
    //    const std::string_view& InEntryPoint,
    //    const EShaderCompilerTargetFormat& InTargetFormat,
    //    const bool& bInShouldOptimize);
};

SHADERCOMPILER_API extern CGlobalShaderCompiler* GShaderCompiler;

}