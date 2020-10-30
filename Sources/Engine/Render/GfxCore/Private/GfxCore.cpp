#include "Module/Module.h"
#include "Gfx/Backend.h"

ZE_DEFINE_MODULE(ze::module::DefaultModule, GfxCore);

namespace ze::gfx
{

RenderBackend* backend = nullptr;

RenderBackend::RenderBackend() { backend = this; }
RenderBackend::~RenderBackend() = default;

RenderBackend& RenderBackend::get()
{
	ZE_CHECK(backend);
	return *backend;
}

}