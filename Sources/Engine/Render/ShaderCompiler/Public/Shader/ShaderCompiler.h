#pragma once

#include "EngineCore.h"
#include "Shader/ShaderCore.h"
#include <future>
#include <string_view>
#include "NonCopyable.h"
#include <robin_hood.h>

namespace ze::gfx::shaders
{

/**
 * Target formats
 */
enum class ShaderCompilerTargetFormat
{
    SpirV,
};

/**
 * Reflection data output
 */
struct ShaderCompilerReflectionDataOutput
{
    ShaderParameterMap parameter_map;
};

/**
 * Output of the shader compiler
 */
struct ShaderCompilerOutput
{
    std::string err_msg;
    bool success;
    std::vector<uint32_t> bytecode;
    ShaderCompilerReflectionDataOutput reflection_data;

    ShaderCompilerOutput() : success(false) {}
};

enum class ShaderCompilerTarget
{
    VulkanSpirV
};

/**
 * Shader compiler interface
 * Should be implemented for each target
 */
class SHADERCOMPILER_API ShaderCompiler
{
public:
    ShaderCompiler(const ShaderCompilerTarget& InTarget);
    virtual ~ShaderCompiler() = default;

	virtual ShaderCompilerOutput compile_shader(
		const ShaderStage& stage,
		const std::string_view& shader_filename,
		const std::string_view& entry_point,
		const ShaderCompilerTarget& target_format,
		const bool& optimize) = 0;
protected:
    ShaderCompilerTarget target;
};

#define ZE_DEFINE_SHADER_COMPILER(Target, Class) static Class* ShaderCompiler_##Class = new Class(Target)

SHADERCOMPILER_API void register_shader_compiler(ShaderCompilerTarget target, OwnerPtr<ShaderCompiler> compiler);

/*
 * Compile a shader (async operation)
 * \return Future shader compiler output
 */
SHADERCOMPILER_API std::future<ShaderCompilerOutput> compile_shader(const ShaderStage& stage,
    const std::string_view& shader_filename,
    const std::string_view& entry_point,
    const ShaderCompilerTarget& target,
    const bool& should_optimize);

}