#pragma once

#include "module/Module.h"
#include "ShaderCompiler.h"

namespace ze::gfx
{
	
/**
 * Base class for modules that host a shader compiler
 */
class ShaderCompilerModule : public module::Module
{
public:
	void initialize() override;

	virtual OwnerPtr<ShaderCompiler> create_shader_compiler() const { return nullptr; }
};
	
}