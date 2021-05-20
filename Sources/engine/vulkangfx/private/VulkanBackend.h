#pragma once

#include "gfx/Backend.h"
#include "Vulkan.h"

namespace ze::gfx::vulkan
{

class Device;

class VulkanBackend final : public Backend
{
public:
	~VulkanBackend();

	std::pair<bool, std::string> initialize() override;
	void device_wait_idle() override;
	void new_frame() override;
	BackendFeatureFlags get_features() const override
	{
		return BackendFeatureFlagBits::CommandPoolTrimming;
	}

	std::pair<Result, ResourceHandle> buffer_create(const BufferCreateInfo& in_create_info) override;
	std::pair<Result, ResourceHandle> swapchain_create(const SwapChainCreateInfo& in_create_info) override; 
	std::pair<Result, ResourceHandle> texture_create(const TextureCreateInfo& in_create_info) override;
	std::pair<Result, ResourceHandle> texture_create(const vk::Image& in_image, const TextureCreateInfo& in_create_info);
	std::pair<Result, ResourceHandle> texture_view_create(const TextureViewCreateInfo& in_create_info) override;
	std::pair<Result, ResourceHandle> texture_view_create(const vk::Image& in_image,
		const TextureViewCreateInfo& in_create_info);
	std::pair<Result, ResourceHandle> shader_create(const ShaderCreateInfo& in_create_info) override;
	std::pair<Result, ResourceHandle> render_pass_create(const RenderPassCreateInfo& in_create_info) override;
	std::pair<Result, ResourceHandle> gfx_pipeline_create(const GfxPipelineCreateInfo& in_create_info) override;
	std::pair<Result, ResourceHandle> pipeline_layout_create(const PipelineLayoutCreateInfo& in_create_info) override;
	std::pair<Result, ResourceHandle> descriptor_set_create(const vk::DescriptorSet& in_set);
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
	void descriptor_set_destroy(const ResourceHandle& in_handle);
	void sampler_destroy(const ResourceHandle& in_handle) override;

	ResourceHandle pipeline_layout_allocate_descriptor_set(const ResourceHandle& in_pipeline_layout,
		const uint32_t in_set,
		const std::vector<Descriptor>& descriptors) override;

	/** Buffer */
	std::pair<Result, void*> buffer_map(const ResourceHandle& in_buffer) override;
	void buffer_unmap(const ResourceHandle& in_buffer) override;

	/** Swap chain */
	bool swapchain_acquire_image(const ResourceHandle& in_swapchain) override;
	void swapchain_resize(const ResourceHandle& in_swapchain, const uint32_t in_new_width,
		const uint32_t in_new_height) override;
	ResourceHandle swapchain_get_backbuffer(const ResourceHandle& in_swapchain) override;
	ResourceHandle swapchain_get_backbuffer_texture(const ResourceHandle& in_swapchain) override;
	uint32_t swapchain_get_backbuffer_index(const ResourceHandle& in_swapchain) override;
	uint32_t swapchain_get_textures_count(const ResourceHandle& in_swapchain) override;
	std::vector<ResourceHandle> swapchain_get_backbuffer_textures(const ResourceHandle& in_swapchain) override;
	std::vector<ResourceHandle> swapchain_get_backbuffer_texture_views(const ResourceHandle& in_swapchain) override;
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
	std::pair<Result, ResourceHandle> fence_create(const bool in_is_signaled = false) override;
	void fence_wait_for(const std::vector<ResourceHandle>& in_fences,
		const bool in_wait_all = true, const uint64_t in_timeout = std::numeric_limits<uint64_t>::max()) override;
	void fence_reset(const std::vector<ResourceHandle>& in_fences) override;
	std::pair<Result, ResourceHandle> semaphore_create() override;

	/** Commands */
	ResourceHandle command_pool_create() override;
	void command_pool_reset(const ResourceHandle& in_pool) override;
	void command_pool_trim(const ResourceHandle& in_pool) override;
	void command_pool_free(const ResourceHandle& in_pool, const std::vector<ResourceHandle>& in_lists) override;
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
	void cmd_push_constants(const ResourceHandle& in_cmd_list,
		const ResourceHandle& in_pipeline_layout,
		const ShaderStageFlags in_shader_stage_flags,
		const uint32_t in_offset,
		const uint32_t in_size,
		const void* in_values) override;
	/** Transfers cmd */
	void cmd_copy_buffer(const ResourceHandle& in_cmd_list,
        const ResourceHandle& in_src_buffer,
        const ResourceHandle& in_dst_buffer,
        const std::vector<BufferCopyRegion>& in_regions) override;
	void cmd_copy_buffer_to_texture(const ResourceHandle& in_cmd_list,
		const ResourceHandle& in_src_buffer,
		const ResourceHandle& in_dst_texture,
		const TextureLayout& in_dst_layout,
		const std::vector<BufferTextureCopyRegion>& in_regions) override;
	void cmd_copy_texture(const ResourceHandle& in_cmd_list,
		const ResourceHandle& in_src_texture,
		const TextureLayout in_src_layout,
		const ResourceHandle& in_dst_texture,
		const TextureLayout in_dst_layout,
		const std::vector<TextureCopyRegion>& in_regions) override;
	void cmd_copy_texture_to_buffer(const ResourceHandle& in_cmd_list,
		const ResourceHandle& in_src_texture,
		const TextureLayout in_src_layout,
		const ResourceHandle& in_dst_buffer,
		const std::vector<BufferTextureCopyRegion>& in_regions) override;
	void cmd_blit_texture(const ResourceHandle& in_cmd_list,
		const ResourceHandle& in_src_texture,
		const TextureLayout in_src_layout,
		const ResourceHandle& in_dst_texture,
		const TextureLayout in_dst_layout,
		const std::vector<TextureBlitRegion>& in_regions,
		const Filter& in_filter) override;

	//ZE_FORCEINLINE bool is_valid() const override { return !!instance; }
	ZE_FORCEINLINE vk::Instance& get_instance() { return *instance; }
private:
	bool is_phys_device_usable(const vk::PhysicalDevice& in_device) const;
private:
	vk::UniqueInstance instance;
	vk::DebugUtilsMessengerEXT messenger;
	std::unique_ptr<Device> device;
};

/**
 * Create a resource
 */
template<typename T, typename... Args>
ResourceHandle create_resource(Args&&... args)
{
	T* ptr = new T(std::forward<Args>(args)...);
	if(ptr->is_valid())
	{
		return ResourceHandle { reinterpret_cast<uint64_t>(ptr) };
	}
	
	delete ptr;
	return {};
}

template<typename T>
void delete_resource(const ResourceHandle& handle)
{
#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	/** Call the ::get() function to validate the handle */
	T::get(handle);
#endif
	delete reinterpret_cast<T*>(handle.handle);
}

template<typename T>
ZE_FORCEINLINE T* get_resource(const ResourceHandle& handle)
{
	return reinterpret_cast<T*>(handle.handle);
}

VulkanBackend& get_backend();

}