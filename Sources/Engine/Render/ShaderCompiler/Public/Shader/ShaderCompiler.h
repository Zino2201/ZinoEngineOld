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

class ShaderCompiler;

SHADERCOMPILER_API ShaderCompiler* register_shader_compiler(ShaderCompilerTarget target, OwnerPtr<ShaderCompiler> compiler);
SHADERCOMPILER_API void unregister_shader_compiler(ShaderCompiler* compiler);

/*
 * Compile a shader
 * \return Future shader compiler output
 */
SHADERCOMPILER_API ShaderCompilerOutput compile_shader(const ShaderStage& stage,
    const std::string& shader_name, 
    const std::string_view& shader_source,
    const std::string_view& entry_point,
    const ShaderCompilerTarget& target,
    const bool& should_optimize);

SHADERCOMPILER_API std::future<ShaderCompilerOutput> compile_shader_async(const ShaderStage& stage,
    const std::string& shader_name,
    const std::string_view& shader_source,
    const std::string_view& entry_point,
    const ShaderCompilerTarget& target,
    const bool& should_optimize);


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
		const std::string_view& shader_name,
		const std::string_view& shader_source,
		const std::string_view& entry_point,
		const ShaderCompilerTarget& target_format,
		const bool& optimize) = 0;

    ZE_FORCEINLINE ShaderCompilerTarget get_target() { return target; }
protected:
    ShaderCompilerTarget target;
};

}