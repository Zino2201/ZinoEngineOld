#include "Shader/ShaderCompiler.h"
#include "Module/Module.h"
#include "Threading/JobSystem/Async.h"

ZE_DEFINE_MODULE(ze::module::DefaultModule, ShaderCompiler)

namespace ze::gfx::shaders
{

robin_hood::unordered_map<ShaderCompilerTarget, std::unique_ptr<ShaderCompiler>> shader_compilers;

ShaderCompiler::ShaderCompiler(const ShaderCompilerTarget& in_target) : target(in_target) {}

void register_shader_compiler(ShaderCompilerTarget target, OwnerPtr<ShaderCompiler> compiler)
{
	ZE_ASSERT(shader_compilers.find(target) == shader_compilers.end());
	shader_compilers.insert({ target, std::unique_ptr<ShaderCompiler>(compiler) });
}

std::future<ShaderCompilerOutput> compile_shader(const ShaderStage& stage,
    const std::string_view& shader_filename,
    const std::string_view& entry_point,
    const ShaderCompilerTarget& target,
    const bool& optimize)
{
	auto it = shader_compilers.find(target);

	if(it != shader_compilers.end())
	{
		ze::logger::info("Compiling shader ({}) {}",
			stage == ShaderStage::Vertex ? "Vertex" : "Fragment",
			shader_filename);

		// TODO: SET DEFINES

		std::string path;
		path += "Shaders/";
		path += shader_filename;

		return ze::jobsystem::async<ShaderCompilerOutput>(
			[it, stage, path = std::move(path), entry_point,
				target, optimize](const ze::jobsystem::Job& in_job)
			{
				ShaderCompilerOutput output = it->second->compile_shader(
					stage,
					path.c_str(),
					entry_point,
					target,
					optimize);

				if(output.success)
					ze::logger::verbose("Shader {} compiled!",
						path.c_str());
				else
					ze::logger::error("Failed to compile shader {} !",
						path.c_str());

				return output;
			});
	}
	else
	{
		ze::logger::error("Failed to compile shader {}: unsupported format",
			shader_filename.data());
		return {};
	}
}

}