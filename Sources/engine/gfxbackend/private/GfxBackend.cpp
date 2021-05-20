#include "gfx/Backend.h"
#include "module/Module.h"

ZE_DEFINE_MODULE(ze::module::DefaultModule, GfxBackend)

namespace ze::gfx
{

Backend* backend = nullptr;

Backend::Backend() { backend = this; }
Backend::~Backend() = default;

Backend& Backend::get()
{
	ZE_CHECK(backend);
	return *backend;
}

}