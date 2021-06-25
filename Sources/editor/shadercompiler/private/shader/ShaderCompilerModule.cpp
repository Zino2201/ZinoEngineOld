#include "shader/ShaderCompilerModule.h"

namespace ze::gfx
{

void ShaderCompilerModule::initialize()
{
	register_shader_compiler(create_shader_compiler());	
}

}