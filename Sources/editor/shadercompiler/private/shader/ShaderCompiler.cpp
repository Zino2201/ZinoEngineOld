#include "shader/ShaderCompiler.h"
#include "module/Module.h"
#include "threading/jobsystem/Async.h"
#include "MessageBox.h"

ZE_DEFINE_MODULE(ze::module::DefaultModule, shadercompiler)

namespace ze::gfx
{

robin_hood::unordered_map<ShaderLanguage, std::unique_ptr<ShaderCompiler>> shader_compilers;

void register_shader_compiler(OwnerPtr<ShaderCompiler> compiler)
{
	ZE_CHECK(compiler);
	ZE_ASSERT(shader_compilers.find(compiler->get_shader_language()) == shader_compilers.end());
	shader_compilers.insert({ compiler->get_shader_language(), std::unique_ptr<ShaderCompiler>(compiler) });
	logger::info("Registered shader compiler {} (shader language: {})", compiler->get_name(), 
		std::to_string(compiler->get_shader_language()));
}

void unregister_shader_compiler(ShaderCompiler* compiler)
{
	ZE_ASSERT(compiler);
	shader_compilers.erase(compiler->get_shader_language());
}

ShaderCompilerOutput compile_shader(const ShaderStageFlagBits stage,
	const std::string& shader_name, 
    const std::string_view& shader_source,
    const std::string_view& entry_point,
    const ShaderFormat target,
    const bool should_optimize)
{
	auto it = shader_compilers.find(target.language);
	
	if(it != shader_compilers.end())
	{
		ze::logger::info("Compiling shader ({}) {}", shader_name);

		ShaderCompilerOutput output = it->second->compile_shader(
			stage,
			shader_name,
			shader_source,
			entry_point,
			target,
			should_optimize);

		return output;
	}
	else
	{
		ze::logger::error("Failed to compile shader {}: unsupported format",
			shader_name.data());
		return {};
	}
}

std::future<ShaderCompilerOutput> compile_shader_async(const ShaderStageFlagBits stage,
  	const std::string& shader_name, 
    const std::string_view& shader_source,
    const std::string_view& entry_point,
    const ShaderFormat format,
    const bool optimize)
{
	auto it = shader_compilers.find(format.language);

	if(it != shader_compilers.end())
	{
		ze::logger::info("Compiling shader {}", shader_name);

		return ze::jobsystem::async<ShaderCompilerOutput>(
			[it, stage, shader_name, source = std::move(shader_source), entry_point_copy = std::string(entry_point),
				format, optimize](const ze::jobsystem::Job& in_job)
			{
				ShaderCompilerOutput output = it->second->compile_shader(
					stage,
					shader_name,
					source,
					entry_point_copy,
					format,
					optimize);
	
				return output;
			});
	}
	
	ze::logger::error("Failed to compile shader {}: unsupported format",
		shader_name.data());

	/** TODO: This is dumb */
	return ze::jobsystem::async<ShaderCompilerOutput>(
[](const ze::jobsystem::Job& in_job)
	{
		return ShaderCompilerOutput();
	});
}

}