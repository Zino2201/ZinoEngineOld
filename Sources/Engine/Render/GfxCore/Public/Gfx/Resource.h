#pragma once

#include <cstdint>

namespace ze::gfx
{

enum class ResourceType
{
	None,
	Buffer,
	Texture,
	TextureView,
	SwapChain,

	/**
	 * A queue to submit command lists to
	 */
	Queue,

	/**
	 * A pool of command lists to allocate from
	 */
	CommandPool,

	/**
	 * A list of command 
	 */
	CommandList,

	/**
	 * Synchronization primitive used for GPU -> GPU synchronization
	 */
	Semaphore,

	/**
	 * Synchronization primitive used for GPU -> CPU synchronization
	 */
	Fence,

	Shader,

	/**
	 * Define the layout of a pipeline (what descriptors are used and when they are located)
	 */
	PipelineLayout,
	
	Pipeline,

	/** 
	 * A render pass
	 */
	RenderPass,

	/** 
	 * A group of descriptors
	 */
	DescriptorSet,

	Sampler
};

/**
 * Handle to a backend resource
 */
struct ResourceHandle
{
	static constexpr uint64_t null = -1;
	
	ResourceType type;
	uint64_t handle;
	
	ResourceHandle(ResourceType in_type = ResourceType::None, uint64_t in_handle = null) : type(in_type),
		 handle(in_handle) {}

	bool operator==(const ResourceHandle& other) const
	{
		return handle == other.handle;
	}

	operator bool() const
	{
		return handle != null;
	}
};

}