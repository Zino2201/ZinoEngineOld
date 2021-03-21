#pragma once

#include "Gfx/Backend.h"
#include <robin_hood.h>
#include "Containers/SparseArray.h"
#include <queue>
#include <bitset>

namespace std
{
	template<> struct hash<std::vector<ze::gfx::Descriptor>>
	{
		ZE_FORCEINLINE uint64_t operator()(const std::vector<ze::gfx::Descriptor>& in_descriptors) const
		{
			uint64_t hash = 0;

			for(const auto& descriptor : in_descriptors)
				ze::hash_combine(hash, descriptor);

			return hash;
		}
	};
}

namespace ze::gfx
{

/** High-level API */

enum class DeviceResourceType : uint64_t
{
	Buffer,
	Texture,
	TextureView,
	SwapChain,
	Fence,
	Semaphore,
	Sampler,
	Shader,
	PipelineLayout,
	Max = 0b1111111111,
};

/**
 * Handle to a device resource
 * First 54-bit store the handle
 * Last 10-bit the resource type for safety
 */
struct DeviceResourceHandle
{
	static constexpr uint64_t null = -1;

	uint64_t handle;

	DeviceResourceHandle() : handle(null) {}
	DeviceResourceHandle(const size_t& in_idx) : handle(in_idx) {}

	static inline DeviceResourceHandle make(const uint64_t& in_handle, const DeviceResourceType& in_type)
	{
		DeviceResourceHandle handle;
		handle.handle = (in_handle << 54) | static_cast<uint64_t>(in_type);
		return handle;
	}

	ZE_FORCEINLINE bool operator==(const DeviceResourceHandle& other) const
	{
		return handle == other.handle;
	}

	ZE_FORCEINLINE bool operator!=(const DeviceResourceHandle& other) const
	{
		return handle != other.handle;
	}

	ZE_FORCEINLINE uint64_t get_index() const
	{
		return handle >> 54;
	}

	ZE_FORCEINLINE DeviceResourceType get_resource_type() const
	{
		return static_cast<DeviceResourceType>(handle & 0xFFFFFFFFFFFFF);
	}

	ZE_FORCEINLINE operator bool() const
	{
		return handle != null;
	}
};

/** Pipeline */

/** 
 * States of the pipeline related to a single render pass
 */
struct GfxPipelineRenderPassState
{
	PipelineMultisamplingStateCreateInfo multisampling;
	PipelineDepthStencilStateCreateInfo depth_stencil;
	PipelineColorBlendStateCreationInfo color_blend;
	uint32_t subpass;
};

struct GfxPipelineShaderStageInfo
{
	ShaderStageFlagBits stage;
	DeviceResourceHandle shader;
	const char* entry_point;

	GfxPipelineShaderStageInfo(const ShaderStageFlagBits& in_stage,
		const DeviceResourceHandle& in_shader,
		const char* in_entry_point) : stage(in_stage), shader(in_shader),
		entry_point(in_entry_point) {}
};

/**
 * States of the pipeline related to a single instance
 */
struct GfxPipelineInstanceState
{
	std::vector<GfxPipelineShaderStageInfo> shaders;
	PipelineVertexInputStateCreateInfo vertex_input;
	PipelineInputAssemblyStateCreateInfo input_assembly;
	PipelineRasterizationStateCreateInfo rasterization;
};

struct RenderPassInfo
{
	std::vector<AttachmentDescription> attachments;
	std::vector<SubpassDescription> subpasses;
	std::array<DeviceResourceHandle, max_attachments_per_framebuffer> color_attachments;
	std::array<DeviceResourceHandle, max_attachments_per_framebuffer> depth_attachments;
	uint32_t width;
	uint32_t height;
	uint32_t layers;
};

/**
 * Type of the command list
 */
enum class CommandListType
{
	Gfx,
	Compute,

	/** Used for CPU => GPU transfers */
	Transfer
};

/**
 * A command list
 */
class CommandList
{
public:
	static constexpr int max_descriptor_sets = 4;
	static constexpr int max_bindings = 16;

	CommandList(const CommandListType& in_type,
		const ResourceHandle& in_handle);

	void begin();
	void end();
	void reset();

	/** Gfx */
	void begin_render_pass(const RenderPassInfo& in_info,
		const maths::Rect2D& in_render_area,
		const std::vector<ClearValue>& in_clear_values);
	void next_subpass();
	void end_render_pass();
	void set_pipeline_render_pass_state(const GfxPipelineRenderPassState& in_state);
	void set_pipeline_instance_state(const GfxPipelineInstanceState& in_state);
	void bind_vertex_buffer(DeviceResourceHandle in_buffer,
		const uint64_t in_offset);
	void bind_vertex_buffers(const uint32_t in_first_binding,
		const std::vector<DeviceResourceHandle> in_buffers,
		const std::vector<uint64_t> in_offsets = {});
	void bind_index_buffer(const DeviceResourceHandle& in_buffer,
		const uint64_t in_offset = 0,
		const IndexType in_type = IndexType::Uint32);
	void set_scissor(const maths::Rect2D& in_scissor);
	void set_scissors(const uint32_t in_first_scissor,
		const std::vector<maths::Rect2D> in_scissors);
	void set_viewport(const Viewport& in_viewport);
	void draw(const uint32_t in_vertex_count,
		const uint32_t in_instance_count,
		const uint32_t in_first_vertex,
		const uint32_t in_first_instance);
	void draw_indexed(const uint32_t in_index_count,
		const uint32_t in_instance_count,
		const uint32_t in_first_index,
		const int32_t in_vertex_offset,
		const uint32_t in_first_instance);

	/** Resource binding */
	void bind_ubo(const uint32_t in_set, const uint32_t in_binding, const DeviceResourceHandle in_buffer);
	void bind_ssbo(const uint32_t in_set, const uint32_t in_binding, const DeviceResourceHandle in_buffer);
	void bind_sampler(const uint32_t in_set, const uint32_t in_binding, const DeviceResourceHandle in_sampler);
	void bind_texture(const uint32_t in_set, const uint32_t in_binding, const DeviceResourceHandle in_texture);

	/** Transfer */
	void copy_buffer(const DeviceResourceHandle& in_src_buffer,
        const DeviceResourceHandle& in_dst_buffer);
	void copy_buffer(const DeviceResourceHandle& in_src_buffer,
	    const DeviceResourceHandle& in_dst_buffer,
		const uint64_t& in_src_offset,
		const uint64_t& in_dst_offset,
		const uint64_t& in_size);
	void copy_buffer_to_texture(const DeviceResourceHandle& in_src_buffer,
		const DeviceResourceHandle& in_dst_texture,
		const BufferTextureCopyRegion& in_region);

	/** Pipeline */
	void bind_pipeline_layout(const DeviceResourceHandle& in_handle);
	void texture_barrier(const DeviceResourceHandle& in_texture,
		const PipelineStageFlags& in_src_flags,
		const AccessFlags& in_src_access_flags,
		const TextureLayout& in_old_layout,
		const PipelineStageFlags& in_dst_flags,
		const AccessFlags& in_dst_access_flags,
		const TextureLayout& in_dst_layout,
		const TextureSubresourceRange& in_subresource_range = TextureSubresourceRange(
			TextureAspectFlagBits::Color,
			0,
			1,
			0,
			1));

	ZE_FORCEINLINE ResourceHandle get_backend_handle() const { return handle; }
	ZE_FORCEINLINE CommandListType get_type() const { return type; }
private:
	void process_gfx_pipeline();
	void process_descriptor_sets();
private:
	CommandListType type;
	ResourceHandle handle;
	GfxPipelineRenderPassState render_pass_state;
	GfxPipelineInstanceState instance_state;
	DeviceResourceHandle pipeline_layout;
	ResourceHandle render_pass;
	ResourceHandle gfx_pipeline;

	/** Current bindings */
	std::array<std::array<Descriptor, max_bindings>, max_descriptor_sets> bindings;
	
	/** Sets to update */
	robin_hood::unordered_set<uint32_t> sets_to_update;

	/** Descriptor set cache */
	robin_hood::unordered_map<uint64_t, ResourceHandle> descriptor_sets;
};

/**
 * A hashmap that delete elements that are not accessed
 */
template<typename Key, typename Value, typename Deleter>
struct LifetimeHashMap
{
	struct ValueEntry
	{
		Value value;
		uint8_t frames;

		ValueEntry(const Value& in_value)
			: value(in_value), frames(0)
		{

		}
	};

	using MapType = robin_hood::unordered_map<Key, ValueEntry>;

public:
	static constexpr uint8_t default_lifetime = 8;

	LifetimeHashMap() : max_lifetime(default_lifetime) {}

	void insert(const Key& in_key, const Value& in_value)
	{
		map.insert({ in_key, ValueEntry(std::move(in_value)) });
	}

	Value* find(const Key& in_key)
	{
		auto it = get(in_key);
		if(it != map.end())
			return &it->second.value;

		return nullptr;
	}

	void new_frame()
	{
		for(auto& [key, value] : map)
		{
			if(value.frames++ >= max_lifetime)
			{
				Deleter()(value.value);
				map.erase(key);
			}
		}
	}

	typename MapType::iterator begin()
	{
		return map.begin();
	}

	typename MapType::iterator end()
	{
		return map.end();
	}
private:
	typename MapType::iterator get(const Key& in_key)
	{
		auto it = map.find(in_key);
		if(it != map.end())
		{
			it->second.frames = 0;
			return it;
		}

		return map.end();
	}
private:
	MapType map;
	uint8_t max_lifetime;
};

struct BufferInfo
{
	BufferCreateInfo create_info;

	/**
	 * Used for CPU -> GPU transfers
	 */
	static BufferInfo make_staging_buffer(const uint64_t& in_size)
	{
		return { BufferCreateInfo(in_size, BufferUsageFlagBits::TransferSrc, MemoryUsage::CpuOnly )};
	}

	static BufferInfo make_vertex_buffer(const uint64_t& in_size)
	{
		return { BufferCreateInfo(in_size, BufferUsageFlagBits::VertexBuffer, MemoryUsage::GpuOnly )};
	}

	static BufferInfo make_vertex_buffer_cpu_visible(const uint64_t& in_size)
	{
		return { BufferCreateInfo(in_size, BufferUsageFlagBits::VertexBuffer, MemoryUsage::CpuToGpu )};
	}

	static BufferInfo make_index_buffer(const uint64_t& in_size)
	{
		return { BufferCreateInfo(in_size, BufferUsageFlagBits::IndexBuffer, MemoryUsage::GpuOnly )};
	}

	static BufferInfo make_index_buffer_cpu_visible(const uint64_t& in_size)
	{
		return { BufferCreateInfo(in_size, BufferUsageFlagBits::IndexBuffer, MemoryUsage::CpuToGpu )};
	}

	static BufferInfo make_ubo(const uint64_t& in_size)
	{
		return { BufferCreateInfo(in_size, BufferUsageFlagBits::UniformBuffer, MemoryUsage::CpuToGpu )};
	}

	static BufferInfo make_ssbo(const uint64_t& in_size)
	{
		return { BufferCreateInfo(in_size, BufferUsageFlagBits::StorageBuffer, MemoryUsage::CpuToGpu )};
	}
};

struct TextureInfo
{
	struct InitialData
	{
		std::span<uint8_t> data;
		PipelineStageFlags dst_pipeline_flags;
		TextureLayout dst_layout;
		AccessFlags dst_access_flags;

		InitialData() {}
		InitialData(const std::span<uint8_t> in_data,
			const PipelineStageFlags in_dst_pipeline_flags,
			const TextureLayout in_dst_layout,
			const AccessFlags in_dst_access_flags) : data(in_data),
			dst_pipeline_flags(in_dst_pipeline_flags), dst_layout(in_dst_layout),
			dst_access_flags(in_dst_access_flags) {}
	};

	TextureCreateInfo create_info;

	static TextureInfo make_2d_texture(const uint32_t in_width, const uint32_t in_height,
		const Format in_format, const uint32_t in_mip_levels, const TextureUsageFlags in_usage_flags)
	{
		return { TextureCreateInfo(TextureType::Tex2D,
			MemoryUsage::GpuOnly,
			in_format,
			in_width,
			in_height,
			1,
			in_mip_levels,
			1,
			SampleCountFlagBits::Count1,
			in_usage_flags) };
	}
};

struct TextureViewInfo
{
	TextureViewCreateInfo create_info;

	static TextureViewInfo make_2d_view(const DeviceResourceHandle& in_texture,
		const Format in_format, const TextureSubresourceRange& in_subresource);
};

/**
 * High-level resources
 */
class Buffer
{
public:
	Buffer(const BufferInfo& in_info, const ResourceHandle& in_handle)
		: info(in_info), handle(in_handle) 
	{
		ZE_CHECK(handle);
	}

	~Buffer()
	{
		if(handle)
			Backend::get().buffer_destroy(handle);

		handle = {};
	}

	Buffer(Buffer&& in_other) : info(std::move(in_other.info)), 
		handle(std::exchange(in_other.handle, {})) {}

	void operator=(Buffer&& in_other)
	{
		info = std::move(in_other.info);
		handle = std::exchange(in_other.handle, {});
	}

	ZE_FORCEINLINE const BufferInfo& get_info() const { return info; }
	ZE_FORCEINLINE const ResourceHandle& get_handle() const { return handle; }
private:
	BufferInfo info;
	ResourceHandle handle;
};

class Texture
{
public:
	Texture(const TextureInfo& in_info, const ResourceHandle& in_handle)
		: info(in_info), handle(in_handle) {}

	~Texture()
	{
		if(handle)
			Backend::get().texture_destroy(handle);

		handle = {};
	}

	Texture(Texture&& in_other) : info(std::move(in_other.info)), 
		handle(std::exchange(in_other.handle, {})) {}

	void operator=(Texture&& in_other)
	{
		info = std::move(in_other.info);
		handle = std::exchange(in_other.handle, {});
	}

	ZE_FORCEINLINE const TextureInfo& get_info() const { return info; }
	ZE_FORCEINLINE const ResourceHandle& get_handle() const { return handle; }
private:
	TextureInfo info;
	ResourceHandle handle;
};

class TextureView
{
public:
	TextureView(const TextureViewInfo& in_info, const ResourceHandle& in_handle)
		: info(in_info), handle(in_handle) {}

	~TextureView()
	{
		if(handle)
			Backend::get().texture_view_destroy(handle);

		handle = {};
	}

	TextureView(TextureView&& in_other) : info(std::move(in_other.info)), 
		handle(std::exchange(in_other.handle, {})) {}

	void operator=(TextureView&& in_other)
	{
		info = std::move(in_other.info);
		handle = std::exchange(in_other.handle, {});
	}

	ZE_FORCEINLINE const TextureViewInfo& get_info() const { return info; }
	ZE_FORCEINLINE const ResourceHandle& get_handle() const { return handle; }
private:
	TextureViewInfo info;
	ResourceHandle handle;
};

class Shader
{
public:
	Shader(const ShaderCreateInfo& in_info, const ResourceHandle& in_handle)
		: info(in_info), handle(in_handle) {}

	~Shader()
	{
		if(handle)
			Backend::get().shader_destroy(handle);

		handle = {};
	}

	Shader(Shader&& in_other) : info(std::move(in_other.info)), 
		handle(std::exchange(in_other.handle, {})) {}

	void operator=(Shader&& in_other)
	{
		info = std::move(in_other.info);
		handle = std::exchange(in_other.handle, {});
	}

	ZE_FORCEINLINE const ShaderCreateInfo& get_info() const { return info; }
	ZE_FORCEINLINE const ResourceHandle& get_handle() const { return handle; }
private:
	ShaderCreateInfo info;
	ResourceHandle handle;
};

class PipelineLayout
{
public:
	PipelineLayout(const PipelineLayoutCreateInfo& in_info, const ResourceHandle& in_handle)
		: info(in_info), handle(in_handle) 
	{
		for(size_t i = 0; i < in_info.set_layouts.size(); ++i)
		{
			const auto& set = in_info.set_layouts[i];
			for(const auto& binding : set.bindings)
				bindings_bits[i][binding.binding] = true;
		}
	}

	~PipelineLayout()
	{
		Backend::get().pipeline_layout_destroy(handle);
		handle = {};
	}

	PipelineLayout(PipelineLayout&& in_other) : info(std::move(in_other.info)), 
		handle(std::exchange(in_other.handle, {})) {}

	void operator=(PipelineLayout&& in_other)
	{
		info = std::move(in_other.info);
		handle = std::exchange(in_other.handle, {});
	}

	ZE_FORCEINLINE const PipelineLayoutCreateInfo& get_info() const { return info; }
	ZE_FORCEINLINE const ResourceHandle& get_handle() const { return handle; }

	bool is_binding_valid(const uint32_t in_set, const uint32_t in_binding) const 
	{ 
		return in_binding < bindings_bits[in_set].size() && bindings_bits[in_set][in_binding];
	}
private:
	PipelineLayoutCreateInfo info;
	ResourceHandle handle;
	/** Allow to know which binding is valid in a set */
	std::array<std::bitset<16>, CommandList::max_descriptor_sets> bindings_bits;
};

class Sampler
{
public:
	Sampler(const SamplerCreateInfo& in_info, const ResourceHandle& in_handle)
		: info(in_info), handle(in_handle) {}

	~Sampler()
	{
		if(handle)
			Backend::get().sampler_destroy(handle);

		handle = {};
	}

	Sampler(Sampler&& in_other) : info(std::move(in_other.info)), 
		handle(std::exchange(in_other.handle, {})) {}

	void operator=(Sampler&& in_other)
	{
		info = std::move(in_other.info);
		handle = std::exchange(in_other.handle, {});
	}

	ZE_FORCEINLINE const SamplerCreateInfo& get_info() const { return info; }
	ZE_FORCEINLINE const ResourceHandle& get_handle() const { return handle; }
private:
	SamplerCreateInfo info;
	ResourceHandle handle;
};

class Swapchain
{
public:
	Swapchain(const SwapChainCreateInfo& in_info, const ResourceHandle& in_handle);
	~Swapchain();

	Swapchain(Swapchain&& in_other) : info(std::move(in_other.info)), 
		handle(std::exchange(in_other.handle, {})) {}

	void operator=(Swapchain&& in_other)
	{
		info = std::move(in_other.info);
		handle = std::exchange(in_other.handle, {});
	}

	void create_texture_handles();

	DeviceResourceHandle get_current_texture() const;
	DeviceResourceHandle get_current_texture_view() const;

	ZE_FORCEINLINE const SwapChainCreateInfo& get_info() const { return info; }
	ZE_FORCEINLINE const ResourceHandle& get_handle() const { return handle; }
private:
	SwapChainCreateInfo info;
	ResourceHandle handle;
	std::vector<DeviceResourceHandle> textures;
	std::vector<DeviceResourceHandle> texture_views;
};

class Fence
{
	void wait() {}
};

/*
 * A command pool that allocate a unique command pool for each threads that use it
 */
class ThreadedCommandPool
{
	struct Pool
	{
		ResourceHandle handle;
		std::vector<std::unique_ptr<CommandList>> command_lists;
		size_t free_command_list;

		Pool();
		~Pool();

		CommandList* allocate_cmd_list(CommandListType type);

		void new_frame();
		void trim();
	};
public:
	ThreadedCommandPool(CommandListType type);

	void new_frame();
	void trim();
	void clear();

	CommandList* allocate_cmd_list();
private:
	Pool& get_pool() { return command_pools[std::this_thread::get_id()]; }
private:
	robin_hood::unordered_map<std::thread::id, Pool> command_pools;
	CommandListType type;
};

/**
 * Represents a GPU device
 * This is where you allocate resources
 */
class Device
{
	friend class CommandList;
	friend class Swapchain;

	/**
	 * A frame contains per-frame specific data like synchronization primitives
	 * resources to delete, etc
	 */
	struct Frame
	{
		/** One command pool per thread */
		ThreadedCommandPool gfx_command_pool;

		/** Command lists to submit at next queue submit */
		std::vector<std::unique_ptr<CommandList>> command_lists;
		std::vector<CommandList*> gfx_lists;
		std::vector<CommandList*> compute_lists;
		std::vector<CommandList*> transfer_lists;
		ResourceHandle gfx_fence;
		ResourceHandle compute_fence;
		ResourceHandle transfer_fence;
		ResourceHandle render_finished_semaphore;
		bool gfx_submitted;

		/** Deferred destruction resources */
		std::vector<DeviceResourceHandle> expired_buffers;
		std::vector<DeviceResourceHandle> expired_textures;
		std::vector<DeviceResourceHandle> expired_texture_views;
		std::vector<DeviceResourceHandle> expired_shaders;
		std::vector<DeviceResourceHandle> expired_pipeline_layouts;
		std::vector<DeviceResourceHandle> expired_samplers;
		std::vector<DeviceResourceHandle> expired_swapchains;

		/** Fences to wait at frame start */
		std::vector<ResourceHandle> wait_fences;

		Frame() : gfx_submitted(false), gfx_command_pool(CommandListType::Gfx)
		{
			gfx_lists.reserve(5);
			expired_buffers.reserve(5);
			expired_textures.reserve(5);
			expired_texture_views.reserve(5);
			expired_shaders.reserve(5);
			expired_pipeline_layouts.reserve(5);
			expired_samplers.reserve(5);
			expired_swapchains.reserve(5);
			wait_fences.reserve(5);
			gfx_fence = Backend::get().fence_create();
			compute_fence = Backend::get().fence_create();
			transfer_fence = Backend::get().fence_create();
			render_finished_semaphore = Backend::get().semaphore_create();
		}

		/**
		 * Reset the frame
		 */
		void reset()
		{
			gfx_submitted = false;
			expired_buffers.clear();
			expired_textures.clear();
			expired_texture_views.clear();
			expired_shaders.clear();
			expired_pipeline_layouts.clear();
			expired_samplers.clear();
			expired_swapchains.clear();
		}

		void free_resources();

		void trim_pools()
		{
			gfx_command_pool.trim();
		}
	};

	struct RenderPassDeleter
	{
		void operator()(const ResourceHandle& in_handle)
		{
			Backend::get().render_pass_destroy(in_handle);
		}
	};
public:
	static constexpr size_t max_frames_in_flight = 2;

	Device();

	static Device& get();

	Device(const Device&) = delete;
	void operator=(const Device&) = delete;

	/**
	 * Begin a new frame
	 * Old frame resources will be deleted when a new frame begin
	 */
	void new_frame();

	/** 
	 * End the current frame
	 * This will submit all awaiting commands list into theirs respective queues
	 */
	void end_frame();

	/** Destroy the device */
	void destroy();

	/** Create functions */
	std::pair<Result, DeviceResourceHandle> create_buffer(const BufferInfo& in_info, 
		const std::span<uint8_t>& in_initial_data = {});
	std::pair<Result, DeviceResourceHandle> create_texture(TextureInfo in_info, 
		const TextureInfo::InitialData& in_initial_data = {});
	std::pair<Result, DeviceResourceHandle> create_texture_view(const TextureViewInfo& in_info);
	std::pair<Result, DeviceResourceHandle> create_shader(const gfx::ShaderCreateInfo& in_info);
	std::pair<Result, DeviceResourceHandle> create_pipeline_layout(const gfx::PipelineLayoutCreateInfo& in_info);
	std::pair<Result, DeviceResourceHandle> create_sampler(const gfx::SamplerCreateInfo& in_info);
	std::pair<Result, DeviceResourceHandle> create_swapchain(const gfx::SwapChainCreateInfo& in_info);
	
	/** Deferred destroy functions */
	void destroy_buffer(const DeviceResourceHandle& in_handle);
	void destroy_texture(const DeviceResourceHandle& in_handle);
	void destroy_texture_view(const DeviceResourceHandle& in_handle);
	void destroy_shader(const DeviceResourceHandle& in_handle);
	void destroy_pipeline_layout(const DeviceResourceHandle& in_handle);
	void destroy_sampler(const DeviceResourceHandle& in_handle);
	void destroy_swapchain(const DeviceResourceHandle& in_handle);

	/**
	 * Allocate a command list from this thread's command pool
	 */
	CommandList* allocate_cmd_list(const CommandListType& in_type);

	/** Buffer */
	std::pair<Result, void*> map_buffer(const DeviceResourceHandle& in_buffer);
	void unmap_buffer(const DeviceResourceHandle& in_buffer);

	/** Swapchain */
	bool acquire_swapchain_texture(const DeviceResourceHandle& in_swapchain);
	void resize_swapchain(const DeviceResourceHandle& in_swapchain, const uint32_t in_width,
		const uint32_t in_height);
	DeviceResourceHandle get_swapchain_backbuffer_texture(const DeviceResourceHandle& in_swapchain);
	DeviceResourceHandle get_swapchain_backbuffer_texture_view(const DeviceResourceHandle& in_swapchain);
	void present(const DeviceResourceHandle& in_swapchain);

	/**
	 * Queue submission of a command list
	 * All submissions are processed at the end of the frame
	 */
	void submit(CommandList* in_list);

	/**
	 * Wait the GPU until it has nothing to do
	 * Perform some resources cleanup like command pool trimming
	 */
	void wait_gpu_idle();

	ResourceHandle get_backend_texture(const DeviceResourceHandle& in_texture) const;
	ResourceHandle get_backend_texture_view(const DeviceResourceHandle& in_texture_view) const;
private:
	ResourceHandle create_or_find_render_pass(const RenderPassCreateInfo& in_create_info);
	ResourceHandle create_or_find_gfx_pipeline(const GfxPipelineRenderPassState& in_render_pass_state,
		const GfxPipelineInstanceState& in_instance_state, ResourceHandle in_render_pass, ResourceHandle in_pipeline_layout);
	void submit_queue(const CommandListType& in_type);
	
	Buffer* get_buffer(const DeviceResourceHandle& in_handle);
	PipelineLayout* get_pipeline_layout(const DeviceResourceHandle& in_handle);
	Sampler* get_sampler(const DeviceResourceHandle& in_handle);
	Texture* get_texture(const DeviceResourceHandle& in_handle);
	TextureView* get_texture_view(const DeviceResourceHandle& in_handle);
	Swapchain* get_swapchain(const DeviceResourceHandle& in_handle);
	Shader* get_shader(const DeviceResourceHandle& in_handle);
	Frame& get_current_frame() { return frames[current_frame]; }
private:
	LifetimeHashMap<RenderPassCreateInfo, ResourceHandle, RenderPassDeleter> render_passes;
	robin_hood::unordered_map<GfxPipelineCreateInfo, ResourceHandle> gfx_pipelines;
	SparseArray<Buffer> buffers;
	SparseArray<Texture> textures;
	SparseArray<TextureView> texture_views;
	SparseArray<Shader> shaders;
	SparseArray<PipelineLayout> pipeline_layouts;
	SparseArray<Sampler> samplers;
	SparseArray<Swapchain> swapchains;
	std::array<Frame, max_frames_in_flight> frames;
	size_t current_frame;
};

/** Smart handles */
template<DeviceResourceType Type, typename Deleter>
class UniqueDeviceResourceHandle
{
public:
	UniqueDeviceResourceHandle() = default;
	explicit UniqueDeviceResourceHandle(const DeviceResourceHandle& in_handle)
	{
		reset(in_handle);
	}

	~UniqueDeviceResourceHandle()
	{
		destroy();
	}

	void operator=(const DeviceResourceHandle& in_handle)
	{
		reset(in_handle);
	}

	UniqueDeviceResourceHandle(const UniqueDeviceResourceHandle& other) = delete;
	void operator=(const UniqueDeviceResourceHandle& other) = delete;

	/** Move ctor/op= */
	UniqueDeviceResourceHandle(UniqueDeviceResourceHandle&& other)
	{
		reset(std::move(other.handle));
	}

	void operator=(UniqueDeviceResourceHandle&& other)
	{
		reset(std::move(other.handle));
	}

	DeviceResourceHandle get()
	{
		ZE_CHECKF(handle, "Tried to get an invalid handle");
		return handle;
	}

	UniqueDeviceResourceHandle free()
	{
		UniqueDeviceResourceHandle freed_handle = handle;
		handle = {};
		return freed_handle;
	}

	void reset(const DeviceResourceHandle& in_new_handle = DeviceResourceHandle())
	{
		if(in_new_handle)
			ZE_CHECKF(in_new_handle.get_resource_type() == Type, "Bad type provided to UniqueDeviceResourceHandle");
		destroy();
		handle = in_new_handle;
	}

	ZE_FORCEINLINE const DeviceResourceHandle& operator*() const
	{
		ZE_CHECKF(handle, "Tried to dereference an invalid handle");
		return handle;
	}

	ZE_FORCEINLINE operator bool() const
	{
		return handle;
	}
private:
	void destroy()
	{
		if (!handle)
			return;
		Deleter()(handle);
		handle = {};
	}
private:
	DeviceResourceHandle handle;
};

namespace detail
{

struct BufferDeleter
{
	void operator()(const DeviceResourceHandle& in_handle) const
	{
		Device::get().destroy_buffer(in_handle);
	}
};

struct TextureDeleter
{
	void operator()(const DeviceResourceHandle& in_handle) const
	{
		Device::get().destroy_texture(in_handle);
	}
};

struct TextureViewDeleter
{
	void operator()(const DeviceResourceHandle& in_handle) const
	{
		Device::get().destroy_texture_view(in_handle);
	}
};

struct ShaderDeleter
{
	void operator()(const DeviceResourceHandle& in_handle) const
	{
		Device::get().destroy_shader(in_handle);
	}
};

struct PipelineLayoutDeleter
{
	void operator()(const DeviceResourceHandle& in_handle) const
	{
		Device::get().destroy_pipeline_layout(in_handle);
	}
};

struct SwapchainDeleter
{
	void operator()(const DeviceResourceHandle& in_handle) const
	{
		Device::get().destroy_swapchain(in_handle);
	}
};

}

using UniqueBuffer = UniqueDeviceResourceHandle<DeviceResourceType::Buffer, detail::BufferDeleter>;
using UniqueTexture = UniqueDeviceResourceHandle<DeviceResourceType::Texture, detail::TextureDeleter>;
using UniqueTextureView = UniqueDeviceResourceHandle<DeviceResourceType::TextureView, detail::TextureViewDeleter>;
using UniqueShader = UniqueDeviceResourceHandle<DeviceResourceType::Shader, detail::ShaderDeleter>;
using UniquePipelineLayout = UniqueDeviceResourceHandle<DeviceResourceType::PipelineLayout, detail::PipelineLayoutDeleter>;
using UniqueSwapchain = UniqueDeviceResourceHandle<DeviceResourceType::SwapChain, detail::SwapchainDeleter>;

struct StaticSamplerBase
{
	static DeviceResourceHandle register_sampler(DeviceResourceHandle sampler);
	static void destroy_samplers();
};

/** Static states helpers */
template<Filter min_filter, Filter mag_filter, 
	Filter mip_map_mode,
	SamplerAddressMode address_mode_u,
	SamplerAddressMode address_mode_v,
	SamplerAddressMode address_mode_w,
	int32_t mip_lod_bias,
	bool enable_compare_op,
	CompareOp compare_op,
	bool enable_anisotropy,
	int32_t max_anisotropy,
	int32_t min_lod,
	int32_t max_lod>
struct StaticSampler
{
	StaticSampler()
	{
		handle = StaticSamplerBase::register_sampler(Device::get().create_sampler(
			SamplerCreateInfo(min_filter,
				mag_filter,
				mip_map_mode,
				address_mode_u,
				address_mode_v,
				address_mode_w,
				mip_lod_bias,
				enable_compare_op,
				compare_op,
				enable_anisotropy,
				max_anisotropy,
				min_lod,
				max_lod)).second);
	}

	StaticSampler(const StaticSampler&) = delete;
	void operator=(const StaticSampler&) = delete;

	static StaticSampler& get()
	{
		static StaticSampler instance;
		return instance;
	}

	ZE_FORCEINLINE DeviceResourceHandle get_handle() const
	{
		return handle;
	}

	operator DeviceResourceHandle() const
	{
		return get_handle();
	}
private:
	DeviceResourceHandle handle;
};

using LinearSampler = StaticSampler<Filter::Linear,
	Filter::Linear, 
	Filter::Linear,
	SamplerAddressMode::Repeat,
	SamplerAddressMode::Repeat,
	SamplerAddressMode::Repeat,
	1,
	false,
	CompareOp::Never,
	false,
	1,
	0,
	1>;

using NearestSampler = StaticSampler<Filter::Nearest,
	Filter::Nearest, 
	Filter::Nearest,
	SamplerAddressMode::Repeat,
	SamplerAddressMode::Repeat,
	SamplerAddressMode::Repeat,
	1,
	false,
	CompareOp::Never,
	false,
	1,
	0,
	1>;

}