#pragma once

#include "EngineCore.h"
#include "Shader/ShaderCore.h"
#include <future>
#include <string_view>
#include "NonCopyable.h"

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

static std::unordered_map<EShaderCompilerTarget, std::unique_ptr<IShaderCompiler>> ShaderCompilers;

/**
 * Global shader compiler
 */
class SHADERCOMPILER_API CGlobalShaderCompiler : public CNonCopyable
{
public:
    static CGlobalShaderCompiler& Get()
    {
        static CGlobalShaderCompiler Instance;
        return Instance;
    }

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
};

}