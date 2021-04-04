#include "Gfx/Gfx.h"

namespace ze::gfx
{

CommandList::CommandList(const CommandListType& in_type,
	const ResourceHandle& in_handle)
	: type(in_type), handle(in_handle)
{

}

void CommandList::begin()
{
	Backend::get().command_list_begin(handle);
}

void CommandList::end()
{
	Backend::get().command_list_end(handle);
}

void CommandList::reset()
{
	render_pass_state = {};
	instance_state = {};
	pipeline_layout = {};
	render_pass = {};
	gfx_pipeline = {};
}

void CommandList::begin_render_pass(const RenderPassInfo& in_render_pass,
	const maths::Rect2D& in_render_area,
	const std::vector<ClearValue>& in_clear_values)
{
	render_pass = Device::get().create_or_find_render_pass(RenderPassCreateInfo(in_render_pass.attachments, in_render_pass.subpasses));
	gfx::Framebuffer framebuffer;

	for(size_t i = 0; i < in_render_pass.color_attachments.size(); ++i)
	{
		if(in_render_pass.color_attachments[i])
			framebuffer.color_attachments[i] = Device::get().get_texture_view(in_render_pass.color_attachments[i])->get_handle();

		if(in_render_pass.depth_attachments[i])
			framebuffer.depth_attachments[i] = Device::get().get_texture_view(in_render_pass.depth_attachments[i])->get_handle();
	}

	framebuffer.width = in_render_pass.width;
	framebuffer.height = in_render_pass.height;
	framebuffer.layers = in_render_pass.layers;

	Backend::get().cmd_begin_render_pass(handle,
		render_pass,
		framebuffer,
		in_render_area,
		in_clear_values);
}

void CommandList::next_subpass()
{
	ZE_ASSERTF(false, "Unimplemented");
}

void CommandList::end_render_pass()
{
	Backend::get().cmd_end_render_pass(handle);
}

/** Transfers */
void CommandList::copy_buffer(const DeviceResourceHandle& in_src_buffer,
    const DeviceResourceHandle& in_dst_buffer,
	const uint64_t& in_src_offset,
	const uint64_t& in_dst_offset,
	const uint64_t& in_size)
{
	Buffer* src_buffer = Device::get().get_buffer(in_src_buffer);
	Buffer* dst_buffer = Device::get().get_buffer(in_dst_buffer);

	Backend::get().cmd_copy_buffer(handle,
		src_buffer->get_handle(),
		dst_buffer->get_handle(),
		{ BufferCopyRegion(in_src_offset, in_dst_offset, in_size) });
}

void CommandList::copy_buffer(const DeviceResourceHandle& in_src_buffer,
    const DeviceResourceHandle& in_dst_buffer)
{
	ZE_CHECK(in_src_buffer && in_dst_buffer);
	Buffer* src_buffer = Device::get().get_buffer(in_src_buffer);
	Buffer* dst_buffer = Device::get().get_buffer(in_dst_buffer);
	ZE_CHECK(src_buffer->get_info().create_info.size == dst_buffer->get_info().create_info.size);

	copy_buffer(in_src_buffer, in_dst_buffer, 0, 0, src_buffer->get_info().create_info.size);
}

void CommandList::copy_buffer_to_texture(const DeviceResourceHandle& in_src_buffer,
	const DeviceResourceHandle& in_dst_texture,
	const BufferTextureCopyRegion& in_region)
{
	Buffer* src_buffer = Device::get().get_buffer(in_src_buffer);
	Texture* dst_texture = Device::get().get_texture(in_dst_texture);
	Backend::get().cmd_copy_buffer_to_texture(handle, 
		src_buffer->get_handle(),
		dst_texture->get_handle(),
		TextureLayout::TransferDst,
		{
			in_region
		});
}

void CommandList::texture_barrier(const DeviceResourceHandle& in_texture,
	const PipelineStageFlags& in_src_flags,
	const AccessFlags& in_src_access_flags,
	const TextureLayout& in_old_layout,
	const PipelineStageFlags& in_dst_flags,
	const AccessFlags& in_dst_access_flags,
	const TextureLayout& in_dst_layout,
	const TextureSubresourceRange& in_subresource_range)
{
	Texture* texture = Device::get().get_texture(in_texture);

	Backend::get().cmd_pipeline_barrier(handle,
		in_src_flags,
		in_dst_flags,
		{
			TextureMemoryBarrier(
				texture->get_handle(),
				in_src_access_flags,
				in_dst_access_flags,
				in_old_layout,
				in_dst_layout,
				in_subresource_range)
		});
}

void CommandList::bind_pipeline_layout(const DeviceResourceHandle& in_layout)
{
	pipeline_layout = in_layout;
}

void CommandList::set_pipeline_render_pass_state(const GfxPipelineRenderPassState& in_state)
{
	render_pass_state = in_state;
}

void CommandList::set_pipeline_instance_state(const GfxPipelineInstanceState& in_state)
{
	instance_state = in_state;
}

void CommandList::bind_ubo(const uint32_t in_set, const uint32_t in_binding, const DeviceResourceHandle in_buffer)
{
	Buffer* buffer = Device::get().get_buffer(in_buffer);
	bindings[in_set][in_binding] = Descriptor(DescriptorType::UniformBuffer,
		in_binding,
		DescriptorBufferInfo(buffer->get_handle()));
	sets_to_update.insert(in_set);
}

void CommandList::bind_ssbo(const uint32_t in_set, const uint32_t in_binding, const DeviceResourceHandle in_buffer)
{
	Buffer* buffer = Device::get().get_buffer(in_buffer);
	bindings[in_set][in_binding] = Descriptor(DescriptorType::StorageBuffer,
		in_binding,
		DescriptorBufferInfo(buffer->get_handle()));
	sets_to_update.insert(in_set);
}

void CommandList::bind_sampler(const uint32_t in_set, const uint32_t in_binding, const DeviceResourceHandle in_sampler)
{
	Sampler* sampler = Device::get().get_sampler(in_sampler);
	bindings[in_set][in_binding] = Descriptor(DescriptorType::Sampler,
		in_binding,
		DescriptorTextureInfo(sampler->get_handle()));
	sets_to_update.insert(in_set);
}

void CommandList::bind_texture(const uint32_t in_set, const uint32_t in_binding, const DeviceResourceHandle in_texture)
{
	TextureView* texture = Device::get().get_texture_view(in_texture);
	bindings[in_set][in_binding] = Descriptor(DescriptorType::SampledTexture,
		in_binding,
		DescriptorTextureInfo(texture->get_handle(),
			TextureLayout::ShaderReadOnly));
	sets_to_update.insert(in_set);
}

void CommandList::bind_vertex_buffer(DeviceResourceHandle in_buffer,
	const uint64_t in_offset)
{
	Buffer* buffer = Device::get().get_buffer(in_buffer);

	Backend::get().cmd_bind_vertex_buffers(handle,
		0,
		{ buffer->get_handle() },
		{ in_offset });
}

void CommandList::bind_vertex_buffers(const uint32_t in_first_binding,
	const std::vector<DeviceResourceHandle> in_buffers,
	const std::vector<uint64_t> in_offsets)
{
	std::vector<ResourceHandle> handles;
	handles.reserve(in_buffers.size());
	
	for(const auto& handle : in_buffers)
	{
		Buffer* buffer = Device::get().get_buffer(handle);
		handles.emplace_back(buffer->get_handle());
	}

	Backend::get().cmd_bind_vertex_buffers(handle,
		0,
		handles,
		in_offsets);
}

void CommandList::bind_index_buffer(const DeviceResourceHandle& in_buffer,
	const uint64_t in_offset,
	const IndexType in_type)
{
	Buffer* buffer = Device::get().get_buffer(in_buffer);
	Backend::get().cmd_bind_index_buffer(handle,
		buffer->get_handle(),
		in_offset,
		in_type);
}

void CommandList::set_viewport(const Viewport& in_viewport)
{
	Backend::get().cmd_set_viewport(handle, 0, { in_viewport });
	Backend::get().cmd_set_scissor(handle, 0, { maths::Rect2D({ 0, 0 }, { in_viewport.width, in_viewport.height }) });
}

void CommandList::set_scissor(const maths::Rect2D& in_scissor)
{
	Backend::get().cmd_set_scissor(handle, 0, { in_scissor });
}

void CommandList::set_scissors(const uint32_t in_first_scissor,
	const std::vector<maths::Rect2D> in_scissors)
{
	Backend::get().cmd_set_scissor(handle,
		in_first_scissor,
		in_scissors);
}

void CommandList::process_gfx_pipeline()
{
	ZE_CHECKF(pipeline_layout, "No pipeline layout binded !");

	PipelineLayout* layout = Device::get().get_pipeline_layout(pipeline_layout);
	ResourceHandle pipeline = Device::get().create_or_find_gfx_pipeline(render_pass_state, instance_state,
		render_pass, layout->get_handle());
	if(gfx_pipeline != pipeline)
	{
		gfx_pipeline = pipeline;
		Backend::get().cmd_bind_pipeline(handle,
			PipelineBindPoint::Gfx,
			gfx_pipeline);
	}
}

void CommandList::process_descriptor_sets()
{
	ZE_CHECKF(pipeline_layout, "No pipeline layout binded !");
	PipelineLayout* layout = Device::get().get_pipeline_layout(pipeline_layout);

	std::vector<ResourceHandle> handles;
	handles.reserve(4);

	for(const auto& set : sets_to_update)
	{
		std::vector<Descriptor> descriptors;
		descriptors.reserve(max_bindings);
		for(const auto& binding : bindings[set])
		{
			if(layout->is_binding_valid(set, binding.dst_binding))
			{
				descriptors.emplace_back(binding);
			}
		}

		handles.emplace_back(Backend::get().pipeline_layout_allocate_descriptor_set(layout->get_handle(),
			set,
			descriptors));
	}

	if(!handles.empty())
	{
		Backend::get().cmd_bind_descriptor_sets(handle, 
			gfx::PipelineBindPoint::Gfx,
			layout->get_handle(),
			0,
			handles);
	}

	sets_to_update.clear();
}

void CommandList::draw(const uint32_t in_vertex_count,
	const uint32_t in_instance_count,
	const uint32_t in_first_vertex,
	const uint32_t in_first_instance)
{
	process_gfx_pipeline();
	process_descriptor_sets();
	Backend::get().cmd_draw(handle, in_vertex_count, in_instance_count, in_first_vertex, in_first_instance);
}

void CommandList::draw_indexed(const uint32_t in_index_count,
	const uint32_t in_instance_count,
	const uint32_t in_first_index,
	const int32_t in_vertex_offset,
	const uint32_t in_first_instance)
{
	process_gfx_pipeline();
	process_descriptor_sets();
	Backend::get().cmd_draw_indexed(handle, 
		in_index_count, 
		in_instance_count, 
		in_first_index, 
		in_vertex_offset,
		in_first_instance);
}

}