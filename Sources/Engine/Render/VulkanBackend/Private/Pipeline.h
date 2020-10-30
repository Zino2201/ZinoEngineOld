#pragma once

#include "Vulkan.h"
#include "Gfx/Backend.h"

namespace ze::gfx::vulkan
{

class Device;

class Pipeline
{
public:
	Pipeline(Device& in_device, const GfxPipelineCreateInfo& in_create_info);

	static Pipeline* get(const ResourceHandle& in_handle);

	ZE_FORCEINLINE bool is_valid() const { return !!pipeline ;}
	ZE_FORCEINLINE vk::Pipeline& get_pipeline() { return *pipeline; }
private:
	Device& device;
	vk::UniquePipeline pipeline;
};

}