#pragma once

#include "EngineCore.h"
#include "shader/ShaderCore.h"
#include <future>
#include "gfx/ShaderFormat.h"
#include "gfx/Gfx.h"

namespace ze::gfx
{

/**
 * Reflection data output
 */
struct ShaderCompilerReflectionDataOutput
{
    ShaderParameterMap parameter_map;

    template<typename ArchiveType>
    void serialize(ArchiveType& archive)
    {
        archive <=> parameter_map;
    }
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


class ShaderCompiler;

void register_shader_compiler(OwnerPtr<ShaderCompiler> compiler);
void unregister_shader_compilers();

/*
 * Compile a shader
 * \return Future shader compiler output
 */
// remove
ShaderCompilerOutput compile_shader(const ShaderStageFlagBits stage,
    const std::string& shader_name, 
    const std::string_view& shader_source,
    const std::string_view& entry_point,
    const ShaderFormat format,
    const bool should_optimize);

std::future<ShaderCompilerOutput> compile_shader_async(const ShaderStageFlagBits stage,
    const std::string& shader_name,
    const std::string_view& shader_source,
    const std::string_view& entry_point,
    const ShaderFormat format,
    const bool should_optimize);

/**
 * Shader compiler interface
 * Should be implemented for each target
 */
class ShaderCompiler
{
public:
    ShaderCompiler(const std::string& in_name, const ShaderLanguage in_shader_language)
		: name(in_name), shader_language(in_shader_language) {}
    virtual ~ShaderCompiler() = default;

	virtual ShaderCompilerOutput compile_shader(
		const ShaderStageFlagBits stage,
		const std::string_view& shader_name,
		const std::string_view& shader_source,
		const std::string_view& entry_point,
		const ShaderFormat format,
		const bool optimize) = 0;

    ZE_FORCEINLINE const std::string& get_name() const { return name; }
    ZE_FORCEINLINE ShaderLanguage get_shader_language() const { return shader_language; }
protected:
	std::string name;
    ShaderLanguage shader_language;
};

}