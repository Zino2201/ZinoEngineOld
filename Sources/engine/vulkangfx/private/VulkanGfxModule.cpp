#include "gfx/BackendModule.h"
#include "Device.h"
#include "VulkanBackend.h"

namespace ze::gfx::vulkan
{

class VulkanGfxModule final : public gfx::BackendModule
{
public:
	Backend* create_backend(const BackendInfo* in_backend, const BackendShaderModel in_requested_shader_model) override
	{
		VulkanBackend* backend = new VulkanBackend(in_backend);
		auto ret = backend->initialize();
		if(!ret.first)
		{
			logger::error(ret.second);
			delete backend;
			backend = nullptr;
		}
		
		return backend;
	}
};
		
}

ZE_DEFINE_MODULE(ze::gfx::vulkan::VulkanGfxModule, vulkangfx);