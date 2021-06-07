#include "gfx/Backend.h"
#include "module/Module.h"
#include "gfx/BackendInfo.h"

ZE_DEFINE_MODULE(ze::module::DefaultModule, gfxbackend)

namespace ze::gfx
{

Backend* backend = nullptr;

Backend::Backend(const BackendInfo* in_backend_info) : backend_info(in_backend_info) { backend = this; }
Backend::~Backend() = default;

ShaderFormat Backend::get_shader_format(const BackendShaderModel in_shader_model) const
{
	return backend_info->get_shader_format(in_shader_model);
}

Backend& Backend::get()
{
	ZE_CHECK(backend);
	return *backend;
}

}