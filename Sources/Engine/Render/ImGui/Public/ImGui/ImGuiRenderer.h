#pragma once

#include "Gfx/Gfx.h"
#include "ImGui/ImGui.h"
#include "Gfx/UniformBuffer.h"

namespace ze::ui::imgui
{

struct WindowData
{
	uint32_t width;
	uint32_t height;
	gfx::UniqueSwapchain swapchain;
};

struct GlobalData
{
	maths::Vector2f scale;
	maths::Vector2f translate;
};

struct ViewportDrawData
{
	gfx::UniformBuffer<GlobalData> global_data;
	gfx::UniqueBuffer vertex_buffer;
	gfx::UniqueBuffer index_buffer;
	size_t vertex_buffer_size;
	size_t index_buffer_size;

	ViewportDrawData() : vertex_buffer_size(0), index_buffer_size(0) {}
};

struct ViewportData
{
	gfx::UniqueTexture texture;
	gfx::UniqueTextureView view;
	WindowData window;
	ViewportDrawData draw_data;
	bool owned_by_renderer;
	gfx::UniqueSemaphore render_finished_semaphore;
	bool has_submitted_work;

	ViewportData() 
		: owned_by_renderer(true), has_submitted_work(false)
	{
		render_finished_semaphore = gfx::Device::get().create_semaphore().second;
	}
};

bool initialize();
void update_viewport_buffers(ImDrawData* draw_data, ViewportDrawData& vp_draw_data);
void draw_viewport(ImDrawData* draw_data, 
	const ViewportData& in_viewport,
	gfx::CommandList* list);
void draw_viewports();
void swap_viewports_buffers();

/** Callbacks, exposed for editor use */
void imgui_create_window_callback(ImGuiViewport* viewport);
void imgui_render_window_callback(ImGuiViewport* viewport, void* unused = nullptr);
void imgui_swap_buffers_callback(ImGuiViewport* viewport, void* unused = nullptr);

void destroy();
}