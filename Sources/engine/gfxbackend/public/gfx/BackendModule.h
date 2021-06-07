#pragma once

#include "module/Module.h"
#include "BackendManager.h"

namespace ze::gfx
{

class Backend;

/**
 * Base class for modules hosting backends
 */
class BackendModule : public module::Module
{
public:
	virtual Backend* create_backend(const BackendInfo* in_backend, const BackendShaderModel in_requested_shader_model) { return nullptr; }
};
	
}