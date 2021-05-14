#include "RenderPass.h"
#include "Device.h"
#include "VulkanBackend.h"
#include <robin_hood.h>
#include "VulkanUtil.h"
#include "Command.h"
#include "Texture.h"

namespace ze::gfx::vulkan
{

struct FramebufferEntry
{
	static constexpr uint8_t framebuffer_lifetime = 10;

	uint8_t lifetime;
	vk::UniqueFramebuffer framebuffer;

	FramebufferEntry(vk::UniqueFramebuffer&& in_fb) : lifetime(0), framebuffer(std::move(in_fb)) {}
};

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
robin_hood::unordered_set<ResourceHandle> render_passes;
#endif
robin_hood::unordered_map<Framebuffer, FramebufferEntry> framebuffers;
vk::Result last_render_pass_result;

std::pair<Result, ResourceHandle> VulkanBackend::render_pass_create(const RenderPassCreateInfo& in_create_info)
{
	ResourceHandle handle = create_resource<RenderPass>(*device, in_create_info);

	RenderPass render_pass(*device, in_create_info);
#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	render_passes.insert(handle);
#endif

	return { convert_vk_result(last_render_pass_result), handle };
}

vk::Framebuffer get_or_create_framebuffer(Device& in_device, const Framebuffer& in_framebuffer,
	const vk::RenderPass& in_render_pass)
{
	auto it = framebuffers.find(in_framebuffer);

	if(it != framebuffers.end())
	{
		it->second.lifetime = 0;
		return *it->second.framebuffer;
	}

	std::vector<vk::ImageView> attachments;
	attachments.reserve(max_attachments_per_framebuffer * 2);
	for(const auto& attachment : in_framebuffer.color_attachments)
	{
		if(!attachment)
			continue;

		TextureView* view = TextureView::get(attachment);
		ZE_CHECKF(view, "Invalid texture view (color attachment) given to framebuffer");
		attachments.emplace_back(view->get_image_view());
	}

	for (const auto& attachment : in_framebuffer.depth_attachments)
	{
		if (!attachment)
			continue;

		TextureView* view = TextureView::get(attachment);
		ZE_CHECKF(view, "Invalid texture view (depth/stencil attachment) given to framebuffer");
		attachments.emplace_back(view->get_image_view());
	}

	auto [result, unique_fb] = in_device.get_device().createFramebufferUnique(
		vk::FramebufferCreateInfo(
			vk::FramebufferCreateFlags(),
			in_render_pass,
			attachments,
			in_framebuffer.width,
			in_framebuffer.height,
			in_framebuffer.layers));
	
	vk::Framebuffer fb = *unique_fb;
	
	if(result != vk::Result::eSuccess)
		ze::logger::error("Failed to create framebuffer: {}", vk::to_string(result));
	else
	{
		framebuffers.insert({ in_framebuffer, std::move(unique_fb) });
	}

	return fb;
}

void VulkanBackend::render_pass_destroy(const ResourceHandle& in_handle)
{
	delete_resource<RenderPass>(in_handle);

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	render_passes.erase(in_handle);
#endif
}

/** Holds attachments */
struct SubpassAttachmentsHolders
{
	std::vector<vk::AttachmentReference> input_attachments;
	std::vector<vk::AttachmentReference> color_attachments;
	std::vector<vk::AttachmentReference> resolve_attachments;
	std::vector<vk::AttachmentReference> depth_stencil_attachments;
	std::vector<uint32_t> preserve_attachments;
};

RenderPass::RenderPass(Device& in_device, const RenderPassCreateInfo& in_create_info)
	: device(in_device), create_info(in_create_info)
{
	std::vector<vk::AttachmentDescription> attachments;
	attachments.reserve(in_create_info.attachments.size());
	for(const auto& attachment : in_create_info.attachments)
	{
		attachments.emplace_back(
			vk::AttachmentDescriptionFlags(),
			convert_format(attachment.format),
			convert_sample_count_bit(attachment.samples),
			convert_load_op(attachment.load_op),
			convert_store_op(attachment.store_op),
			convert_load_op(attachment.stencil_load_op),
			convert_store_op(attachment.stencil_store_op),
			convert_texture_layout(attachment.initial_layout),
			convert_texture_layout(attachment.final_layout));
	}

	std::vector<SubpassAttachmentsHolders> holders;
	std::vector<vk::SubpassDescription> subpasses;
	subpasses.reserve(in_create_info.subpasses.size());
	holders.reserve(subpasses.size());

	for(const auto& subpass : in_create_info.subpasses)
	{
		holders.emplace_back();
		auto& holder = holders.back();

		for(const auto& attachment : subpass.input_attachments)
			holder.input_attachments.emplace_back(attachment.attachment, 
				convert_texture_layout(attachment.layout));

		for(const auto& attachment : subpass.color_attachments)
			holder.color_attachments.emplace_back(attachment.attachment, 
				convert_texture_layout(attachment.layout));
		
		for(const auto& attachment : subpass.resolve_attachments)
			holder.resolve_attachments.emplace_back(attachment.attachment, 
				convert_texture_layout(attachment.layout));
		
		for(const auto& attachment : subpass.preserve_attachments)
			holder.preserve_attachments.emplace_back(attachment);

		vk::AttachmentReference depth_stencil_attachment(subpass.depth_stencil_attachment.attachment, 
			convert_texture_layout(subpass.depth_stencil_attachment.layout));

		subpasses.emplace_back(
			vk::SubpassDescriptionFlags(),
			vk::PipelineBindPoint::eGraphics,
			holder.input_attachments,
			holder.color_attachments,
			holder.resolve_attachments,
			subpass.depth_stencil_attachment.attachment == AttachmentReference::unused_attachment ? nullptr : &depth_stencil_attachment,
			vk::ArrayProxyNoTemporaries<const uint32_t>(holder.preserve_attachments.size(), holder.preserve_attachments.data()));
	}

	auto [result, handle] = device.get_device().createRenderPassUnique(
		vk::RenderPassCreateInfo(
			vk::RenderPassCreateFlags(),
			attachments,
			subpasses));
	if(result != vk::Result::eSuccess)
		ze::logger::error("Failed to create render pass: {}", vk::to_string(result));

	render_pass = std::move(handle);
	last_render_pass_result = result;
}

RenderPass* RenderPass::get(const ResourceHandle& in_handle)
{
#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	auto render_pass = render_passes.find(in_handle);
	ZE_CHECKF(render_pass != render_passes.end(), "Invalid render pass");
#endif

	return get_resource<RenderPass>(in_handle);
}

void destroy_framebuffers()
{
	framebuffers.clear();
}

void update_framebuffers()
{
	std::vector<Framebuffer> key_to_deletes;

	for(auto& [key, val] : framebuffers)
	{
		val.lifetime++;
		if(val.lifetime >= FramebufferEntry::framebuffer_lifetime)
			key_to_deletes.emplace_back(key);
	}

	for(const auto& key : key_to_deletes)
		framebuffers.erase(key);
}

}