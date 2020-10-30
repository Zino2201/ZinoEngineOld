#include "Command.h"
#include "Pipeline.h"
#include "VulkanBackend.h"
#include "RenderPass.h"
#include "Texture.h"
#include "Buffer.h"
#include "VulkanUtil.h"
#include "Device.h"
#include "PipelineLayout.h"
#include "DescriptorSet.h"

namespace ze::gfx::vulkan
{

/** Implementation of all commands */

void VulkanBackend::cmd_pipeline_barrier(const ResourceHandle& in_cmd_list,
	const PipelineStageFlags& in_src_flags,
	const PipelineStageFlags& in_dst_flags,
	const std::vector<TextureMemoryBarrier>& in_texture_memory_barriers)
{
	CommandList* list = CommandList::get(in_cmd_list);
	ZE_CHECKF(list, "Invalid command list given to cmd_pipeline_barrier");

	vk::PipelineStageFlags src_stage = convert_pipeline_stage_flags(in_src_flags);
	vk::PipelineStageFlags dst_stage = convert_pipeline_stage_flags(in_dst_flags);

	std::vector<vk::ImageMemoryBarrier> image_memory_barriers;
	image_memory_barriers.reserve(in_texture_memory_barriers.size());

	for(const auto& barrier : in_texture_memory_barriers)
	{
		Texture* tex = Texture::get(barrier.texture);
		ZE_CHECKF(tex, "Invalid texture given to cmd_pipeline_barrier");

		image_memory_barriers.emplace_back(convert_access_flags(barrier.src_access_flags),
			convert_access_flags(barrier.dst_access_flags), 
			convert_texture_layout(barrier.old_layout), convert_texture_layout(barrier.new_layout),
			VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
			tex->get_image(), convert_texture_subresource_range(barrier.subresource_range));
	}

	list->get_buffer().pipelineBarrier(src_stage, dst_stage,
		vk::DependencyFlags(),
		{},
		{},
		image_memory_barriers);
}

void VulkanBackend::cmd_begin_render_pass(const ResourceHandle& in_cmd_list,
	const ResourceHandle& in_render_pass,
	const Framebuffer& in_framebuffer,
	const maths::Rect2D& in_render_area,
	const std::vector<ClearValue>& in_clear_values)
{
	CommandList* list = CommandList::get(in_cmd_list);
	ZE_CHECKF(list, "Invalid command list given to cmd_begin_render_pass");

	RenderPass* render_pass = RenderPass::get(in_render_pass);
	ZE_CHECKF(render_pass, "Invalid render pass given to cmd_begin_render_pass");

	std::vector<vk::ClearValue> clear_values;
	clear_values.resize(in_clear_values.size());
	memcpy(clear_values.data(), in_clear_values.data(), sizeof(ClearValue) * in_clear_values.size());

	list->get_buffer().beginRenderPass(
		vk::RenderPassBeginInfo(
			render_pass->get_render_pass(),
			get_or_create_framebuffer(*device, in_framebuffer, render_pass->get_render_pass()),
			convert_rect2D(in_render_area),
			clear_values),
		vk::SubpassContents::eInline);
}

void VulkanBackend::cmd_end_render_pass(const ResourceHandle& in_cmd_list)
{
	CommandList* list = CommandList::get(in_cmd_list);
	ZE_CHECKF(list, "Invalid command list given to cmd_begin_render_pass");

	list->get_buffer().endRenderPass();
}

void VulkanBackend::cmd_bind_pipeline(const ResourceHandle& in_cmd_list,
	const PipelineBindPoint& in_bind_point,
	const ResourceHandle& in_pipeline)
{
	CommandList* list = CommandList::get(in_cmd_list);
	ZE_CHECKF(list, "Invalid command list given to cmd_bind_pipeline");

	Pipeline* pipeline = Pipeline::get(in_pipeline);
	ZE_CHECKF(pipeline, "Invalid pipeline given to cmd_bind_pipeline");

	list->get_buffer().bindPipeline(
		convert_pipeline_bind_point(in_bind_point),
		pipeline->get_pipeline());
}

void VulkanBackend::cmd_bind_vertex_buffers(const ResourceHandle& in_cmd_list,
	const uint32_t in_first_binding, 
	const std::vector<ResourceHandle>& in_buffers,
	const std::vector<uint64_t>& in_offsets)
{
	CommandList* list = CommandList::get(in_cmd_list);
	ZE_CHECKF(list, "Invalid command list given to cmd_bind_vertex_buffers");

	std::vector<vk::Buffer> buffers;
	buffers.reserve(in_buffers.size());
	for(const auto& handle : in_buffers)
	{
		Buffer* buffer = Buffer::get(handle);
		ZE_CHECKF(buffer, "Invalid buffer given to cmd_bind_vertex_buffers");
		buffers.emplace_back(buffer->get_buffer());
	}

	list->get_buffer().bindVertexBuffers(
		in_first_binding,
		buffers,
		in_offsets);
}

void VulkanBackend::cmd_bind_index_buffer(const ResourceHandle& in_cmd_list,
	const ResourceHandle& in_buffer,
	const uint64_t in_offset,
	const IndexType in_type)
{
	CommandList* list = CommandList::get(in_cmd_list);
	ZE_CHECKF(list, "Invalid command list given to cmd_bind_index_buffer");

	Buffer* buffer = Buffer::get(in_buffer);
	ZE_CHECKF(buffer, "Invalid buffer given to cmd_bind_index_buffer");

	list->get_buffer().bindIndexBuffer(buffer->get_buffer(),
		in_offset,
		in_type == IndexType::Uint16 ? vk::IndexType::eUint16 : vk::IndexType::eUint32);
}

void VulkanBackend::cmd_draw(const ResourceHandle& in_cmd_list,
	const uint32_t in_vertex_count,
	const uint32_t in_instance_count,
	const uint32_t in_first_vertex,
	const uint32_t in_first_instance)
{
	CommandList* list = CommandList::get(in_cmd_list);
	ZE_CHECKF(list, "Invalid command list given to cmd_draw");

	list->get_buffer().draw(in_vertex_count,
		in_instance_count,
		in_first_vertex,
		in_first_instance);
}

void VulkanBackend::cmd_draw_indexed(const ResourceHandle& in_cmd_list,
	const uint32_t in_index_count,
	const uint32_t in_instance_count,
	const uint32_t in_first_index,
	const int32_t in_vertex_offset,
	const uint32_t in_first_instance)
{
	CommandList* list = CommandList::get(in_cmd_list);
	ZE_CHECKF(list, "Invalid command list given to cmd_draw_indexed");

	list->get_buffer().drawIndexed(in_index_count,
		in_instance_count,
		in_first_index,
		in_vertex_offset,
		in_first_instance);
}

void VulkanBackend::cmd_set_viewport(const ResourceHandle& in_cmd_list,
	uint32_t in_first_viewport,
	const std::vector<Viewport>& in_viewports)
{
	CommandList* list = CommandList::get(in_cmd_list);
	ZE_CHECKF(list, "Invalid command list given to cmd_set_viewport");

	list->get_buffer().setViewport(in_first_viewport,
		static_cast<uint32_t>(in_viewports.size()),
		reinterpret_cast<const vk::Viewport*>(in_viewports.data()));
}

void VulkanBackend::cmd_set_scissor(const ResourceHandle& in_cmd_list,
	uint32_t in_first_scissor,
	const std::vector<maths::Rect2D>& in_scissors) 
{
	CommandList* list = CommandList::get(in_cmd_list);
	ZE_CHECKF(list, "Invalid command list given to cmd_set_scissor");

	std::vector<vk::Rect2D> rectangles;
	rectangles.reserve(in_scissors.size());
	for(const auto& scissor : in_scissors)
		rectangles.emplace_back(
			vk::Offset2D(scissor.position.x, scissor.position.y),
			vk::Extent2D(scissor.size.x, scissor.size.y));

	list->get_buffer().setScissor(in_first_scissor,
		rectangles);
}

void VulkanBackend::cmd_bind_descriptor_sets(const ResourceHandle& in_cmd_list,
	const PipelineBindPoint in_bind_point,
	const ResourceHandle& in_pipeline_layout,
	const uint32_t& in_first_set,
	const std::vector<ResourceHandle>& in_descriptor_sets)
{
	CommandList* list = CommandList::get(in_cmd_list);
	ZE_CHECKF(list, "Invalid command list given to cmd_bind_descriptor_sets");

	PipelineLayout* layout = PipelineLayout::get(in_pipeline_layout);
	std::vector<vk::DescriptorSet> sets;
	sets.reserve(in_descriptor_sets.size());
	for(const auto& desc_set : in_descriptor_sets)
	{
		DescriptorSet* set = DescriptorSet::get(desc_set);
		ZE_CHECKF(set, "Invalid descriptor set given to cmd_bind_descriptor_sets");
		sets.emplace_back(set->get_set());
	}
	
	list->get_buffer().bindDescriptorSets(
		convert_pipeline_bind_point(in_bind_point),
		layout->get_layout(),
		in_first_set,
		sets,
		{});
}

/** Transfer commands */
void VulkanBackend::cmd_copy_buffer_to_texture(const ResourceHandle& in_cmd_list,
	const ResourceHandle& in_src_buffer,
	const ResourceHandle& in_dst_texture,
	const TextureLayout& in_dst_layout,
	const std::vector<BufferTextureCopyRegion>& in_regions)
{
	CommandList* list = CommandList::get(in_cmd_list);
	ZE_CHECKF(list, "Invalid command list given to cmd_copy_buffer_to_texture");

	Buffer* src_buffer = Buffer::get(in_src_buffer);
	ZE_CHECKF(src_buffer, "Invalid source buffer given to cmd_copy_buffer_to_texture");
	
	Texture* dst_texture = Texture::get(in_dst_texture);
	ZE_CHECKF(dst_texture, "Invalid destination texture given to cmd_copy_buffer_to_texture");
	
	std::vector<vk::BufferImageCopy> regions;
	regions.reserve(in_regions.size());
	for(const auto& region : in_regions)
	{
		regions.emplace_back(
			region.buffer_offset,
			0,
			0,
			convert_texture_subresource_layers(region.texture_subresource),
			convert_offset3D(region.texture_offset),
			convert_extent3D(region.texture_extent));
	}
	
	list->get_buffer().copyBufferToImage(
		src_buffer->get_buffer(),
		dst_texture->get_image(),
		convert_texture_layout(in_dst_layout),
		regions);
}

}