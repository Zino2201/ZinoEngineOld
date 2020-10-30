#pragma once

#include "Gfx/Backend.h"
#include "Vulkan.h"

namespace ze::gfx::vulkan
{

class Device;

class VULKANBACKEND_API VulkanBackend : public RenderBackend
{
public:
	~VulkanBackend();

	std::pair<bool, std::string> initialize() override;
	void device_wait_idle() override;
	void new_frame() override;

	ResourceHandle buffer_create(const BufferCreateInfo& in_create_info) override;
	ResourceHandle swapchain_create(const SwapChainCreateInfo& in_create_info) override; 
	std::pair<Result, ResourceHandle> texture_create(const TextureCreateInfo& in_create_info) override;
	std::pair<Result, ResourceHandle> texture_create(const vk::Image& in_image, const TextureCreateInfo& in_create_info);
	ResourceHandle texture_view_create(const TextureViewCreateInfo& in_create_info) override;
	ResourceHandle texture_view_create(const vk::Image& in_image,
		const TextureViewCreateInfo& in_create_info);
	std::pair<Result, ResourceHandle> shader_create(const ShaderCreateInfo& in_create_info) override;
	std::pair<Result, ResourceHandle> render_pass_create(const RenderPassCreateInfo& in_create_info) override;
	std::pair<Result, ResourceHandle> gfx_pipeline_create(const GfxPipelineCreateInfo& in_create_info) override;
	std::pair<Result, ResourceHandle> pipeline_layout_create(const PipelineLayoutCreateInfo& in_create_info) override;
	std::pair<Result, ResourceHandle> descriptor_set_create(const DescriptorSetCreateInfo& in_create_info) override;
	std::pair<Result, ResourceHandle> sampler_create(const SamplerCreateInfo& in_create_info) override;

	void buffer_destroy(const ResourceHandle& in_handle) override;
	void texture_destroy(const ResourceHandle& in_handle) override;
	void texture_view_destroy(const ResourceHandle& in_handle) override;
	void swapchain_destroy(const ResourceHandle& in_handle) override;
	void fence_destroy(const ResourceHandle& in_handle) override;
	void semaphore_destroy(const ResourceHandle& in_handle) override;
	void command_pool_destroy(const ResourceHandle& in_handle) override;
	void shader_destroy(const ResourceHandle& in_handle) override;
	void render_pass_destroy(const ResourceHandle& in_handle) override;
	void pipeline_destroy(const ResourceHandle& in_handle) override;
	void pipeline_layout_destroy(const ResourceHandle& in_handle) override;
	void descriptor_set_destroy(const ResourceHandle& in_handle) override;
	void sampler_destroy(const ResourceHandle& in_handle) override;

	/** Buffer */
	std::pair<Result, void*> buffer_map(const ResourceHandle& in_buffer) override;
	void buffer_unmap(const ResourceHandle& in_buffer) override;

	/** Swap chain */
	bool swapchain_acquire_image(const ResourceHandle& in_swapchain) override;
	void swapchain_resize(const ResourceHandle& in_swapchain, const uint32_t in_new_width,
		const uint32_t in_new_height) override;
	ResourceHandle swapchain_get_backbuffer(const ResourceHandle& in_swapchain) override;
	void swapchain_present(const ResourceHandle& in_swapchain,
		const std::vector<ResourceHandle>& in_wait_semaphores) override;

	/** Queues */
	ResourceHandle queue_create(const uint32_t& in_family,
		const uint32_t& in_idx);
	void queue_execute(const ResourceHandle& in_queue, 
		const std::vector<ResourceHandle>& in_command_lists,
		const ResourceHandle& in_fence,
		const std::vector<ResourceHandle>& in_wait_semaphores,
		const std::vector<PipelineStageFlags>& in_wait_stage_flags,
		const std::vector<ResourceHandle>& in_signal_semaphores) override;
	ResourceHandle get_gfx_queue() const override;

	/** Sync */
	ResourceHandle fence_create(const bool in_is_signaled = false) override;
	void fence_wait_for(const std::vector<ResourceHandle>& in_fences,
		const bool in_wait_all = true, const uint64_t in_timeout = std::numeric_limits<uint64_t>::max()) override;
	void fence_reset(const std::vector<ResourceHandle>& in_fences) override;
	ResourceHandle semaphore_create() override;

	/** Commands */
	ResourceHandle command_pool_create(CommandPoolType in_type) override;
	void command_pool_reset(const ResourceHandle& in_pool) override;
	std::vector<ResourceHandle> command_pool_allocate(const ResourceHandle& in_pool, const size_t in_count) override;
	Result command_list_begin(const ResourceHandle& in_command_list) override;
	Result command_list_end(const ResourceHandle& in_command_list) override;

	void cmd_bind_pipeline(const ResourceHandle& in_cmd_list,
		const PipelineBindPoint& in_bind_point,
		const ResourceHandle& in_pipeline) override;
	void cmd_pipeline_barrier(const ResourceHandle& in_cmd_list,
		const PipelineStageFlags& in_src_flags,
		const PipelineStageFlags& in_dst_flags,
		const std::vector<TextureMemoryBarrier>& in_texture_memory_barriers) override;
	void cmd_begin_render_pass(const ResourceHandle& in_cmd_list,
		const ResourceHandle& in_render_pass,
		const Framebuffer& in_framebuffer,
		const maths::Rect2D& in_render_area,
		const std::vector<ClearValue>& in_clear_values) override;
	void cmd_end_render_pass(const ResourceHandle& in_cmd_list) override;
	void cmd_bind_vertex_buffers(const ResourceHandle& in_cmd_list,
		const uint32_t in_first_binding, 
		const std::vector<ResourceHandle>& in_buffers,
		const std::vector<uint64_t>& in_offsets = {}) override;
	void cmd_bind_index_buffer(const ResourceHandle& in_cmd_list,
		const ResourceHandle& in_buffer,
		const uint64_t in_offset = 0,
		const IndexType in_type = IndexType::Uint32) override;
	void cmd_draw(const ResourceHandle& in_cmd_list,
		const uint32_t in_vertex_count,
		const uint32_t in_instance_count,
		const uint32_t in_first_vertex,
		const uint32_t in_first_instance) override;
	void cmd_draw_indexed(const ResourceHandle& in_cmd_list,
		const uint32_t in_index_count,
		const uint32_t in_instance_count,
		const uint32_t in_first_index,
		const int32_t in_vertex_offset,
		const uint32_t in_first_instance) override;
	void cmd_set_viewport(const ResourceHandle& in_cmd_list,
		uint32_t in_first_viewport,
		const std::vector<Viewport>& in_viewports) override;
	void cmd_set_scissor(const ResourceHandle& in_cmd_list,
		uint32_t in_first_scissor,
		const std::vector<maths::Rect2D>& in_scissors) override;
	void cmd_bind_descriptor_sets(const ResourceHandle& in_cmd_list,
		const PipelineBindPoint in_bind_point,
		const ResourceHandle& in_pipeline_layout,
		const uint32_t& in_first_set,
		const std::vector<ResourceHandle>& in_descriptor_sets) override;
	/** Transfers cmd */
	void cmd_copy_buffer_to_texture(const ResourceHandle& in_cmd_list,
		const ResourceHandle& in_src_buffer,
		const ResourceHandle& in_dst_texture,
		const TextureLayout& in_dst_layout,
		const std::vector<BufferTextureCopyRegion>& in_regions) override;

	ZE_FORCEINLINE bool is_valid() const override { return !!instance; }
	ZE_FORCEINLINE vk::Instance& get_instance() { return *instance; }
private:
	bool is_phys_device_usable(const vk::PhysicalDevice& in_device) const;
private:
	vk::UniqueInstance instance;
	vk::DebugUtilsMessengerEXT messenger;
	std::unique_ptr<Device> device;
};

/**
 * Create a resource handle by using Vulkan handle + hashing create info
 */
template<typename T, typename U>
ResourceHandle create_resource_handle(ResourceType in_type, const T& in_c_handle, 
	const U& in_create_info)
{
	static_assert(!std::is_class_v<T>, "Please cast to the C handle");

	uint64_t seed = static_cast<std::underlying_type_t<ResourceType>>(in_type) + reinterpret_cast<uint64_t>(in_c_handle);
	hash_combine(seed, in_create_info);
	return { in_type, seed };
}

VulkanBackend& get_backend();

}