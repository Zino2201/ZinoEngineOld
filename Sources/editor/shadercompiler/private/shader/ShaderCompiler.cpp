#include "shader/ShaderCompiler.h"
#include "module/Module.h"
#include "threading/jobsystem/Async.h"
#include "MessageBox.h"

ZE_DEFINE_MODULE(ze::module::DefaultModule, ShaderCompiler)

namespace ze::gfx::shaders
{

robin_hood::unordered_map<ShaderCompilerTarget, std::unique_ptr<ShaderCompiler>> shader_compilers;

ShaderCompiler::ShaderCompiler(const ShaderCompilerTarget& in_target) : target(in_target) {}

ShaderCompiler* register_shader_compiler(ShaderCompilerTarget target, OwnerPtr<ShaderCompiler> compiler)
{
	ZE_ASSERT(shader_compilers.find(target) == shader_compilers.end());
	shader_compilers.insert({ target, std::unique_ptr<ShaderCompiler>(compiler) });
	return compiler;
}

void unregister_shader_compiler(ShaderCompiler* compiler)
{
	ZE_ASSERT(compiler);
	shader_compilers.erase(compiler->get_target());
}

ShaderCompilerOutput compile_shader(const ShaderStage& stage,
	const std::string& shader_name, 
    const std::string_view& shader_source,
    const std::string_view& entry_point,
    const ShaderCompilerTarget& target,
    const bool& should_optimize)
{
	auto it = shader_compilers.find(target);

	if(it != shader_compilers.end())
	{
		ze::logger::info("Compiling shader ({}) {}",
			stage == ShaderStage::Vertex ? "Vertex" : "Fragment",
			shader_name);

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

std::future<ShaderCompilerOutput> compile_shader_async(const ShaderStage& stage,
  	const std::string& shader_name, 
    const std::string_view& shader_source,
    const std::string_view& entry_point,
    const ShaderCompilerTarget& target,
    const bool& optimize)
{
	auto it = shader_compilers.find(target);

	if(it != shader_compilers.end())
	{
		ze::logger::info("Compiling shader ({}) {}",
			stage == ShaderStage::Vertex ? "Vertex" : "Fragment",
			shader_name);

		return ze::jobsystem::async<ShaderCompilerOutput>(
			[it, stage, shader_name, source = std::move(shader_source), entry_point,
				target, optimize](const ze::jobsystem::Job& in_job)
			{
				ShaderCompilerOutput output = it->second->compile_shader(
					stage,
					shader_name,
					source,
					entry_point,
					target,
					optimize);

				return output;
			});
	}
	else
	{
		ze::logger::error("Failed to compile shader {}: unsupported format",
			shader_name.data());
		return {};
	}
}

}