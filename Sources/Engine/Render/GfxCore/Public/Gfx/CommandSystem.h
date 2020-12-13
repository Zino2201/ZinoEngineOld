#pragma once

#include "EngineCore.h"
#include "Backend.h"
#include "Delegates/Delegate.h"
#include "Delegates/MulticastDelegate.h"

/**
 * \namespace High-level command system
 */
namespace ze::gfx::hlcs
{

/**
 * Queue flags
 * Highest enum as the highest priority on command lists
 */
enum class QueueType : uint8_t
{
	Transfer = 0,
	Compute = 1,
	Gfx = 2,
};

/**
 * Enum of all supported command types
 */
enum class CommandType
{
	/** General commands */
	BindPipeline,
	
	/** Gfx commands */
	BeginRenderPass,
	EndRenderPass,
	BindVertexBuffers,
	BindIndexBuffer,
	BindDescriptorSet,
	PipelineBarrier,
	Draw,
	DrawIndexed,

	/** Transfer commands */
	CopyBuffer,
	CopyBufferToTexture,
	CopyTextureToBuffer,
	CopyTexture,
	BlitTexture
};

struct CommandBase
{
	CommandType type;
};

template<CommandType Type, QueueType TargetQueueType>
struct Command
{
	static constexpr CommandType type = Type;
	static constexpr QueueType queue_type = TargetQueueType;

	void record(const ResourceHandle& in_cmd_list) { static_assert(false, "No record implementation available for this Command type"); }
};

/** Commands */
struct CommandBeginRenderPass : public Command<CommandType::BeginRenderPass, QueueType::Gfx>
{
	ResourceHandle render_pass;
	Framebuffer framebuffer;
	maths::Rect2D render_area;
	std::vector<ClearValue> clear_values;

	void record(const ResourceHandle& in_cmd_list) 
	{
		RenderBackend::get().cmd_begin_render_pass(in_cmd_list,
			render_pass, framebuffer, render_area, clear_values);
	}
};

struct CommandEndRenderPass : public Command<CommandType::EndRenderPass, QueueType::Gfx> 
{
	void record(const ResourceHandle& in_cmd_list) 
	{
		RenderBackend::get().cmd_end_render_pass(in_cmd_list);
	}
};

struct CommandPipelineBarrier : public Command<CommandType::PipelineBarrier, QueueType::Compute> 
{
	PipelineStageFlags src_flags;
	PipelineStageFlags dst_flags;
	std::vector<TextureMemoryBarrier> texture_memory_barriers;

	CommandPipelineBarrier(const PipelineStageFlags& in_src_flags,
		const PipelineStageFlags& in_dst_flags,
		const std::vector<TextureMemoryBarrier>& in_texture_memory_barriers) : src_flags(in_src_flags),
		dst_flags(in_dst_flags), texture_memory_barriers(in_texture_memory_barriers) {}
	
	void record(const ResourceHandle& in_cmd_list) 
	{
		RenderBackend::get().cmd_pipeline_barrier(in_cmd_list,
			src_flags,
			dst_flags,
			texture_memory_barriers);
	}
};

struct CommandCopyBuffer : public Command<CommandType::CopyBuffer, QueueType::Transfer>
{
	ResourceHandle src_buffer;
	ResourceHandle dst_buffer;
	std::vector<BufferCopyRegion> regions;
	
	CommandCopyBuffer(const ResourceHandle& in_src_buffer,
		const ResourceHandle& in_dst_buffer,
		const std::vector<BufferCopyRegion>& in_regions) : src_buffer(in_src_buffer),
		dst_buffer(in_dst_buffer), regions(in_regions) {}

	void record(const ResourceHandle& in_cmd_list) 
	{
		RenderBackend::get().cmd_copy_buffer(in_cmd_list,
			src_buffer,
			dst_buffer,
			regions);
	}
};

struct CommandCopyBufferToTexture : public Command<CommandType::CopyBufferToTexture, QueueType::Transfer> 
{
	ResourceHandle src_buffer;
	ResourceHandle dst_texture;
	TextureLayout dst_layout;
	std::vector<BufferTextureCopyRegion> regions;

	CommandCopyBufferToTexture(const ResourceHandle& in_src_buffer,
		const ResourceHandle& in_dst_texture,
		const TextureLayout& in_dst_layout,
		const std::vector<BufferTextureCopyRegion>& in_regions) : src_buffer(in_src_buffer),
		dst_texture(in_dst_texture), dst_layout(in_dst_layout), regions(in_regions) {}
	
	void record(const ResourceHandle& in_cmd_list) 
	{
		RenderBackend::get().cmd_copy_buffer_to_texture(in_cmd_list,
			src_buffer,
			dst_texture,
			dst_layout,
			regions);
	}
};

struct CommandCopyTextureToBuffer : public Command<CommandType::CopyTextureToBuffer, QueueType::Transfer> 
{
	ResourceHandle src_texture;
	TextureLayout src_layout;
	ResourceHandle dst_buffer;
	std::vector<BufferTextureCopyRegion> regions;

	CommandCopyTextureToBuffer(const ResourceHandle& in_src_texture,
		const TextureLayout in_src_layout,
		const ResourceHandle& in_dst_buffer,
		const std::vector<BufferTextureCopyRegion>& in_regions) : src_texture(in_src_texture),
		src_layout(in_src_layout), dst_buffer(in_dst_buffer), regions(in_regions) {}
	
	void record(const ResourceHandle& in_cmd_list) 
	{
		RenderBackend::get().cmd_copy_texture_to_buffer(in_cmd_list, 
			src_texture,
			src_layout, 
			dst_buffer, 
			regions);
	}
};

struct CommandBlitTexture : public Command<CommandType::BlitTexture, QueueType::Gfx> 
{
	ResourceHandle src_texture;
	TextureLayout src_layout;
	ResourceHandle dst_texture;
	TextureLayout dst_layout;
	std::vector<TextureBlitRegion> regions;
	Filter filter;

	CommandBlitTexture(const ResourceHandle& in_src_texture,
		const TextureLayout in_src_layout,
		const ResourceHandle& in_dst_texture,
		const TextureLayout in_dst_layout,
		const std::vector<TextureBlitRegion>& in_regions,
		const Filter& in_filter) : src_texture(in_src_texture),
			src_layout(in_src_layout), dst_texture(in_dst_texture), dst_layout(in_dst_layout), regions(in_regions),
			filter(in_filter) {}
	
	void record(const ResourceHandle& in_cmd_list) 
	{
		RenderBackend::get().cmd_blit_texture(in_cmd_list, 
			src_texture,
			src_layout,
			dst_texture, 
			dst_layout, 
			regions, filter);
	}
};

class CommandList
{
	friend struct CommandPool;

public:
	CommandList(const size_t in_idx, const bool in_transient = true) 
		: transient(in_transient), need_recording(true), idx(in_idx), should_execute(false)
	{

	}

	CommandList(const CommandList&) = delete;
	void operator=(const CommandList&) = delete;

	CommandList(CommandList&& other) : transient(std::move(other.transient)), handle(std::move(other.handle)),
		need_recording(std::move(other.need_recording)), should_execute(other.should_execute.load()), 
		target_queue(std::move(other.target_queue)), owned_buffers(std::move(other.owned_buffers)), idx(std::move(other.idx)) {}

	ZE_FORCEINLINE void begin()
	{
		if(need_recording)
		{
			RenderBackend::get().command_list_begin(handle);
			should_execute = false;
		}
	}

	ZE_FORCEINLINE void end()
	{
		if(need_recording)
		{
			RenderBackend::get().command_list_end(handle);
			should_execute = true;
		}
	}

	template<typename Command, typename... Args>
	void enqueue(Args&&... in_args)
	{
		if(need_recording)
		{
			Command cmd(std::forward<Args>(in_args)...);
			cmd.record(handle);

			if(target_queue < Command::queue_type)
				target_queue = Command::queue_type;
		}
	}
	
	ZE_FORCEINLINE void mark_as_executed() { should_execute = false; }

	ZE_FORCEINLINE void add_owned_buffer(ResourceHandle in_handle) 
	{ 
		ZE_CHECK(transient);
		owned_buffers.emplace_back(in_handle); 
	}

	ZE_FORCEINLINE ResourceHandle get_handle() const { return handle; } 
	ZE_FORCEINLINE bool should_execute_list() const { return should_execute; }
	ZE_FORCEINLINE bool is_transient() const { return transient; }
private:
	/** Transient (will only be used one time, these commands list are recycled */
	bool transient;
	ResourceHandle handle;
	bool need_recording;
	std::atomic_bool should_execute;
	
	/** Target queue for this list */
	QueueType target_queue;

	/** Buffers owned by this command list */
	std::vector<ResourceHandle> owned_buffers;

	size_t idx;
public:
	DelegateNoRet<> on_executed;
};

GFXCORE_API void initialize();
GFXCORE_API void destroy();

/**
 * Queue a resource for destruction when all commands are submitted
 */
GFXCORE_API void queue_destroy(const gfx::ResourceHandle& in_handle);

/**
 * Allocate a new command list from the current thread command pool
 * \param in_is_transient Is the list transient ? A transient command list is a command list that will be recorded and submitted only once
 */
GFXCORE_API CommandList& allocate_cmd_list(bool in_is_transient = true);

GFXCORE_API void begin_new_frame();

/**
 * Will submit all command lists collected from all pools
 */
GFXCORE_API void submit_all_lists();

/**
 * Submit a single command list
 */
GFXCORE_API void submit(CommandList& in_list, const ResourceHandle& in_fence_to_signal);

}