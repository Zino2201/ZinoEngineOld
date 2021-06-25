#include "imgui/ImGuiRenderer.h"
#include "imgui/ImGui.h"
#include "gfx/Gfx.h"
#include "module/ModuleManager.h"
#include "gfx/effect/EffectDatabase.h"

namespace ze::ui::imgui
{

/** variables */
gfx::DeviceResourceHandle pipeline_layout;
gfx::UniqueTexture font;
gfx::UniqueTextureView font_view;
gfx::UniqueShader vs;
gfx::UniqueShader fs;
void* ubo_data = nullptr;
gfx::GfxPipelineRenderPassState render_pass_state;
gfx::GfxPipelineInstanceState instance_state;

void update_viewport_buffers(ImDrawData* draw_data, ViewportDrawData& vp_draw_data)
{
	if(draw_data->TotalVtxCount == 0)
		return;

	uint64_t vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
	uint64_t index_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);

	/** Create/resize buffers */
	if (!vp_draw_data.vertex_buffer || vp_draw_data.vertex_buffer_size < vertex_size)
	{
		vp_draw_data.vertex_buffer = gfx::Device::get().create_buffer(
			gfx::BufferInfo::make_vertex_buffer_cpu_visible(
				vertex_size)).second;
		if (!vp_draw_data.vertex_buffer)
		{
			ze::logger::error("Failed to create ImGui vertex buffer");
			return;
		}

		vp_draw_data.vertex_buffer_size = vertex_size;
	}

	if (!vp_draw_data.index_buffer || vp_draw_data.index_buffer_size < index_size)
	{
		vp_draw_data.index_buffer = gfx::Device::get().create_buffer(
			gfx::BufferInfo::make_index_buffer_cpu_visible(
				index_size)).second;
		if (!vp_draw_data.index_buffer)
		{
			ze::logger::error("Failed to create ImGui index buffer");
			return;
		}

		vp_draw_data.index_buffer_size = index_size;
	}

	/** Write data to both buffers */
	auto vertex_map = gfx::Device::get().map_buffer(*vp_draw_data.vertex_buffer);
	auto index_map = gfx::Device::get().map_buffer(*vp_draw_data.index_buffer);
	
	ImDrawVert* vertex_data = reinterpret_cast<ImDrawVert*>(vertex_map.second);
	ImDrawIdx* index_data = reinterpret_cast<ImDrawIdx*>(index_map.second);

	for (int i = 0; i < draw_data->CmdListsCount; i++)
	{
		ImDrawList* cmd_list = draw_data->CmdLists[i];
		memcpy(vertex_data, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		memcpy(index_data, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		vertex_data += cmd_list->VtxBuffer.Size;
		index_data += cmd_list->IdxBuffer.Size;
	}

	gfx::Device::get().unmap_buffer(*vp_draw_data.vertex_buffer);
	gfx::Device::get().unmap_buffer(*vp_draw_data.index_buffer);

	/** Update global data */
	GlobalData gd;
	gd.scale = maths::Vector2f(2.0f / draw_data->DisplaySize.x, 2.0f / draw_data->DisplaySize.y);
	gd.translate.x = -1.f - draw_data->DisplayPos.x * gd.scale.x;
	gd.translate.y = -1.f - draw_data->DisplayPos.y * gd.scale.y;
	vp_draw_data.global_data.update(gd);
}

/** Drawing code */
void draw_viewport(ImDrawData* draw_data, 
	const ViewportData& in_viewport,
	gfx::CommandList* list)
{
	/** Update UBO */
	list->bind_pipeline_layout(pipeline_layout);
	list->set_pipeline_render_pass_state(render_pass_state);
	list->set_pipeline_instance_state(instance_state);

	uint32_t vertex_offset = 0;
	uint32_t index_offset = 0;
	uint64_t vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
	uint64_t index_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);

	list->bind_ubo(0, 0, in_viewport.draw_data.global_data.get());
	list->bind_sampler(0, 1, gfx::LinearSampler::get());

	if (draw_data->CmdListsCount > 0)
	{
		list->bind_vertex_buffer(*in_viewport.draw_data.vertex_buffer, 0);
		list->bind_index_buffer(*in_viewport.draw_data.index_buffer, 0, gfx::IndexType::Uint16);

		for (int32_t i = 0; i < draw_data->CmdListsCount; i++)
		{
			ImDrawList* cmd_list = draw_data->CmdLists[i];
			for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
			{
				ImDrawCmd* cmd = &cmd_list->CmdBuffer[j];
				
				ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
				ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

				ImVec4 clip_rect;
                clip_rect.x = std::max<float>((cmd->ClipRect.x - clip_off.x) * clip_scale.x, 0);
                clip_rect.y = std::max<float>((cmd->ClipRect.y - clip_off.y) * clip_scale.y, 0);
                clip_rect.z = (cmd->ClipRect.z - clip_off.x) * clip_scale.x;
                clip_rect.w = (cmd->ClipRect.w - clip_off.y) * clip_scale.y;

				if(cmd->TextureId)
				{
					list->bind_texture(0, 2, *reinterpret_cast<gfx::DeviceResourceHandle*>(cmd->TextureId));
				}
				else
				{
					list->bind_texture(0, 2, *font_view);
				}

				list->set_scissor(maths::Rect2D(
					maths::Vector2f(clip_rect.x,
						clip_rect.y),
					maths::Vector2f(
						clip_rect.z - clip_rect.x,
						clip_rect.w - clip_rect.y)));

				list->draw_indexed(cmd->ElemCount,
					1,
					cmd->IdxOffset + index_offset,
					cmd->VtxOffset + vertex_offset,
					0);
			}

			vertex_offset += cmd_list->VtxBuffer.Size;
			index_offset += cmd_list->IdxBuffer.Size;
		}
	}
}

/** Multi-viewport code */
void imgui_create_window_callback(ImGuiViewport* viewport)
{
	ViewportData* data = new ViewportData;
	viewport->RendererUserData = data;
	data->window.swapchain = gfx::Device::get().create_swapchain(gfx::SwapChainCreateInfo(
		viewport->PlatformHandle, viewport->Size.x, viewport->Size.y)).second;
	data->window.width = viewport->Size.x;
	data->window.height = viewport->Size.y;
	data->window.has_rendered_one_frame.resize(gfx::Device::get().get_swapchain_texture_count(*data->window.swapchain));
}

void imgui_destroy_window(ImGuiViewport* viewport)
{
	ViewportData* data = reinterpret_cast<ViewportData*>(viewport->RendererUserData);
	if(data && data->owned_by_renderer)
	{
		delete data;
		viewport->RendererUserData = nullptr;
	}
}

void imgui_set_window_size(ImGuiViewport* viewport, ImVec2 size)
{
	gfx::Device::get().wait_gpu_idle();
	ViewportData* data = reinterpret_cast<ViewportData*>(viewport->RendererUserData);
	gfx::Device::get().resize_swapchain(*data->window.swapchain,
		size.x, size.y);
	data->window.width = size.x;
	data->window.height = size.y;
	for(auto frame : data->window.has_rendered_one_frame)
	{
		frame = false;
	}

	gfx::Device::get().wait_gpu_idle();
}

void imgui_render_window_callback(ImGuiViewport* viewport, void* list_ptr)
{
	using namespace gfx;
	
	ViewportData* data = reinterpret_cast<ViewportData*>(viewport->RendererUserData);
	update_viewport_buffers(viewport->DrawData, data->draw_data);
	data->has_submitted_work = false;

	if(Device::get().acquire_swapchain_texture(*data->window.swapchain))
	{
		CommandList* list = Device::get().allocate_cmd_list(CommandListType::Gfx);

		gfx::RenderPassInfo render_pass;
		render_pass.attachments = {
			gfx::AttachmentDescription(
				gfx::Format::B8G8R8A8Unorm,
				gfx::SampleCountFlagBits::Count1,
				gfx::AttachmentLoadOp::Load,
				gfx::AttachmentStoreOp::Store,
				gfx::AttachmentLoadOp::DontCare,
				gfx::AttachmentStoreOp::DontCare,
				gfx::TextureLayout::Present,
				gfx::TextureLayout::Present),
		};
		render_pass.subpasses = {
			gfx::SubpassDescription({}, { gfx::AttachmentReference(0, gfx::TextureLayout::ColorAttachment) })
		};
		render_pass.color_attachments[0] = Device::get().get_swapchain_backbuffer_texture_view(*data->window.swapchain);
		render_pass.width = data->window.width;
		render_pass.height = data->window.height;
		render_pass.layers = 1;

		if(!data->window.has_rendered_one_frame[Device::get().get_swapchain_current_idx(*data->window.swapchain)])
		{
			list->texture_barrier(Device::get().get_swapchain_backbuffer_texture(*data->window.swapchain),
				PipelineStageFlagBits::TopOfPipe,
				AccessFlags(),
				TextureLayout::Undefined,
				PipelineStageFlagBits::TopOfPipe,
				AccessFlags(),
				TextureLayout::Present);
			data->window.has_rendered_one_frame[Device::get().get_swapchain_current_idx(*data->window.swapchain)] = true;
		}

		list->begin_render_pass(render_pass,
			maths::Rect2D(maths::Vector2f(),
				maths::Vector2f(data->window.width, data->window.height)),
			{
				gfx::ClearColorValue({0, 0, 0, 1})
			});	
		list->set_viewport(gfx::Viewport(0, 0, data->window.width, data->window.height));
		draw_viewport(viewport->DrawData, 
			*data, 
			list);
		list->end_render_pass();

		Device::get().submit(list, { *data->render_finished_semaphore });
		data->has_submitted_work = true;
	}
}

void imgui_swap_buffers_callback(ImGuiViewport* viewport, void* unused)
{
	using namespace gfx;
	
	ViewportData* data = reinterpret_cast<ViewportData*>(viewport->RendererUserData);
	if(data->has_submitted_work)
	{
		Device::get().present(*data->window.swapchain, { *data->render_finished_semaphore });
	}
}

bool initialize(ImFontAtlas* in_atlas)
{
	ZE_CHECK(in_atlas);

	ImGuiIO& io = ImGui::GetIO();

	/** Setup platform IO bindings */
	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	platform_io.Renderer_CreateWindow = &imgui_create_window_callback;
	platform_io.Renderer_DestroyWindow = &imgui_destroy_window;
	platform_io.Renderer_SetWindowSize = &imgui_set_window_size;
	platform_io.Renderer_RenderWindow = &imgui_render_window_callback;
	platform_io.Renderer_SwapBuffers = &imgui_swap_buffers_callback;
	io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

	/** Shaders */
	gfx::Effect* base_effect = gfx::effect_get_by_name("ImGui");
	auto* permutation = base_effect->get_permutation({});
	while(!base_effect->is_available({})) {}
	ZE_CHECK(permutation);
	pipeline_layout = *permutation->pipeline_layout;

	/**
	 * Create font texture
	 */
	uint8_t* data = nullptr;
	uint32_t width = 0;
	uint32_t height = 0;
	in_atlas->GetTexDataAsRGBA32(&data, reinterpret_cast<int*>(&width), 
		reinterpret_cast<int*>(&height));
	auto [result, handle] = gfx::Device::get().create_texture(
		gfx::TextureInfo::make_2d_texture(
			width, 
			height, 
			gfx::Format::R8G8B8A8Unorm,
			1,
			gfx::TextureUsageFlagBits::Sampled),
		gfx::TextureInfo::InitialData(std::span<uint8_t>(data, width * height * 4),
			gfx::PipelineStageFlagBits::FragmentShader,
			gfx::TextureLayout::ShaderReadOnly,
			gfx::AccessFlagBits::ShaderRead));
	if(result != gfx::Result::Success)
	{
		ze::logger::error("Failed to create ImGui font texture");
		return false;
	}

	font = handle;

	font_view = gfx::Device::get().create_texture_view(
		gfx::TextureViewInfo::make_2d_view(
			*font,
			gfx::Format::R8G8B8A8Unorm,
			gfx::TextureSubresourceRange(gfx::TextureAspectFlagBits::Color,
				0,
				1,
				0,
				1))).second;

	render_pass_state.color_blend = gfx::PipelineColorBlendStateCreationInfo(
		false, gfx::LogicOp::NoOp,
		{
			gfx::PipelineColorBlendAttachmentState(true,
				gfx::BlendFactor::SrcAlpha,
				gfx::BlendFactor::OneMinusSrcAlpha,
				gfx::BlendOp::Add,
				gfx::BlendFactor::OneMinusSrcAlpha,
				gfx::BlendFactor::Zero,
				gfx::BlendOp::Add)
		});

	instance_state.shaders = {
		gfx::GfxPipelineShaderStageInfo(
			gfx::ShaderStageFlagBits::Vertex,
			permutation->shader_map[gfx::ShaderStageFlagBits::Vertex],
			"vertex"),
		gfx::GfxPipelineShaderStageInfo(
			gfx::ShaderStageFlagBits::Fragment,
			permutation->shader_map[gfx::ShaderStageFlagBits::Fragment],
			"fragment"),
	};
	instance_state.vertex_input = gfx::PipelineVertexInputStateCreateInfo(
		{
			gfx::VertexInputBindingDescription(0, 
				sizeof(ImDrawVert),
				gfx::VertexInputRate::Vertex)
		},
		{
			gfx::VertexInputAttributeDescription(0, 0, 
				gfx::Format::R32G32Sfloat, offsetof(ImDrawVert, pos)),
			gfx::VertexInputAttributeDescription(1, 0, 
				gfx::Format::R32G32Sfloat, offsetof(ImDrawVert, uv)),
			gfx::VertexInputAttributeDescription(2, 0,
				gfx::Format::R8G8B8A8Unorm, offsetof(ImDrawVert, col)),
		});

	return true;
}

void destroy()	
{
	font.reset();
	font_view.reset();
	vs.reset();
	fs.reset();
}

void draw_viewports()
{
	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    for (int i = 1; i < platform_io.Viewports.Size; i++)
    {
        ImGuiViewport* viewport = platform_io.Viewports[i];
        if (viewport->Flags & ImGuiViewportFlags_Minimized)
            continue;
        if (platform_io.Platform_RenderWindow) platform_io.Platform_RenderWindow(viewport, nullptr);
        if (platform_io.Renderer_RenderWindow) platform_io.Renderer_RenderWindow(viewport, nullptr);
    }

}

void swap_viewports_buffers()
{
	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    for (int i = 1; i < platform_io.Viewports.Size; i++)
    {
        ImGuiViewport* viewport = platform_io.Viewports[i];
        if (viewport->Flags & ImGuiViewportFlags_Minimized)
            continue;
        if (platform_io.Platform_SwapBuffers) platform_io.Platform_SwapBuffers(viewport, nullptr);
        if (platform_io.Renderer_SwapBuffers) platform_io.Renderer_SwapBuffers(viewport, nullptr);
    }
}

}