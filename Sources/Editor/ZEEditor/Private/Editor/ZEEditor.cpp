#include "Editor/ZEEditor.h"
#include "Module/Module.h"
#include "Engine/Viewport.h"
#include "ImGui/ImGui.h"
#include "backends/imgui_impl_sdl.h"
#include <SDL.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define UUID_SYSTEM_GENERATOR
#include "Assets/Asset.h"
#include "AssetDatabase/AssetDatabase.h"
#include <istream>
#include "Editor/Assets/AssetFactory.h"
#include "Assets/AssetManager.h"
#include "Editor/AssetUtils/AssetUtils.h"
#include "ZEFS/FileStream.h"
#include "Gfx/Gfx.h"
#include <SDL.h>
#include <sstream>
#include "Gfx/GpuVector.h"
#include "Engine/NativeWindow.h"
#include "Editor/Windows/MapEditor.h"
#include "Editor/IconManager.h"
#include "Editor/Windows/Window.h"
#include "Editor/Assets/AssetActions.h"
#include "Editor/Assets/AssetFactory.h"
#include "ZEFS/Paths.h"
#include "ZEFS/Utils.h"
#include <filesystem>
#include "Editor/Windows/Console.h"
#include <imgui_internal.h>
#include "Editor/PropertyEditor.h"
#include "Editor/PropertyEditors/PrimitivesPropertyEditors.h"
#include "Assets/AssetMetadata.h"
#include "Serialization/Json.h"
#include "Assets/Asset.h"
#include "Assets/AssetCooker.h"
#include "Gfx/Gfx.h"
#include "PlatformMgr.h"
#include "Shader/ShaderCompiler.h"
#include "Maths/Matrix/Transformations.h"
#include "Engine/InputSystem.h"
#include "imgui_internal.h"

ZE_DEFINE_MODULE(ze::module::DefaultModule, ZEEditor);

namespace ze::editor
{

EditorApp* app = nullptr;

EditorApp& EditorApp::get()
{
	return *app;
}

EditorApp::EditorApp() : EngineApp(),
	window(std::make_unique<NativeWindow>("ZinoEngine Editor", 1280, 720,
		0, 0, NativeWindowFlagBits::Centered | NativeWindowFlagBits::Resizable | NativeWindowFlagBits::Maximized)),
	cancel_next_submission(false)
{
	app = this;

	assetdatabase::scan("Assets", assetdatabase::AssetScanMode::Sync);


	/** Main context */
	main_context = ImGui::CreateContext(&font_atlas);

	/** Create seperate imgui context for task window */
	tasks_window_context = ImGui::CreateContext(&font_atlas);
	
	/** Initialize ImGui */

	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	io.ConfigViewportsNoAutoMerge = false;
	io.DisplaySize = ImVec2(static_cast<float>(window->get_width()), static_cast<float>(window->get_height()));
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	io.WantCaptureKeyboard = true;
	io.WantCaptureMouse = true;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
	/** Default ZE editor style */
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 0.f;
		style.FrameRounding = 3.f;
		style.TabRounding = 0.f;
		style.ScrollbarRounding = 0.f;
		style.WindowMenuButtonPosition = ImGuiDir_Right;
		style.TabMinWidthForCloseButton = 0.f;
		style.ItemSpacing = ImVec2(8, 4);
		style.WindowBorderSize = 0.f;
		style.FrameBorderSize = 0.f;
		style.PopupBorderSize = 1.f;
		style.TabBorderSize = 1.f;

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text]                   = ImVec4(0.79f, 0.79f, 0.79f, 1.00f);
		colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg]               = ImVec4(0.22f, 0.22f, 0.22f, 0.94f);
		colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg]                = ImVec4(0.20f, 0.20f, 0.20f, 0.94f);
		colors[ImGuiCol_Border]                 = ImVec4(0.09f, 0.09f, 0.09f, 0.50f);
		colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg]                = ImVec4(0.16f, 0.16f, 0.16f, 0.54f);
		colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.30f, 0.30f, 0.30f, 0.40f);
		colors[ImGuiCol_FrameBgActive]          = ImVec4(0.33f, 0.33f, 0.33f, 0.67f);
		colors[ImGuiCol_TitleBg]                = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		colors[ImGuiCol_TitleBgActive]          = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark]              = ImVec4(0.71f, 0.71f, 0.71f, 1.00f);
		colors[ImGuiCol_SliderGrab]             = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
		colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		colors[ImGuiCol_Button]                 = ImVec4(0.29f, 0.29f, 0.29f, 0.40f);
		colors[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		colors[ImGuiCol_ButtonActive]           = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
		colors[ImGuiCol_Header]                 = ImVec4(0.11f, 0.11f, 0.11f, 0.31f);
		colors[ImGuiCol_HeaderHovered]          = ImVec4(0.13f, 0.13f, 0.13f, 0.80f);
		colors[ImGuiCol_HeaderActive]           = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
		colors[ImGuiCol_Separator]              = ImVec4(0.15f, 0.14f, 0.16f, 0.50f);
		colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.15f, 0.14f, 0.16f, 1.00f);
		colors[ImGuiCol_SeparatorActive]        = ImVec4(0.14f, 0.13f, 0.16f, 1.00f);
		colors[ImGuiCol_ResizeGrip]             = ImVec4(0.00f, 0.00f, 0.00f, 0.25f);
		colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.11f, 0.11f, 0.11f, 0.67f);
		colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.00f, 0.00f, 0.00f, 0.95f);
		colors[ImGuiCol_Tab]                    = ImVec4(0.16f, 0.16f, 0.16f, 0.86f);
		colors[ImGuiCol_TabHovered]             = ImVec4(0.29f, 0.29f, 0.29f, 0.80f);
		colors[ImGuiCol_TabActive]              = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
		colors[ImGuiCol_TabUnfocused]           = ImVec4(0.24f, 0.24f, 0.24f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
		colors[ImGuiCol_DockingPreview]         = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
		colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
		colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
		colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	}

	font = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto-Medium.ttf", 18.f);
	ze::ui::imgui::initialize(&font_atlas);
	ImGui_ImplSDL2_InitForVulkan(reinterpret_cast<SDL_Window*>(window->get_handle()));

	/** Create gfx resources */
	ImGui::SetCurrentContext(main_context);
	{
		ImGui::GetMainViewport()->RendererUserData = &main_viewport_data;
		main_viewport_data.window.swapchain = gfx::Device::get().create_swapchain(gfx::SwapChainCreateInfo(window->get_handle(),
			window->get_width(), window->get_height())).second;
		main_viewport_data.window.width = window->get_width();
		main_viewport_data.window.height = window->get_height();
		main_viewport_data.window.has_rendered_one_frame.resize(gfx::Device::get().get_swapchain_texture_count(*main_viewport_data.window.swapchain));
		main_viewport_data.owned_by_renderer = false;
	}

	ImGui::SetCurrentContext(tasks_window_context);
	{
		ImGui::GetMainViewport()->RendererUserData = &main_viewport_data;
	}

	/** Load default icons */
	load_icon_file("icon-asset-file", "Assets/Icons/icons8-file-64.png");
	load_icon_file("icon-asset-directory", "Assets/Icons/icons8-folder-64.png");

	/** Default windows */
	main_windows.emplace_back(std::make_unique<MapEditor>());
	main_windows.emplace_back(std::make_unique<Console>());

	assetutils::get_on_asset_imported().bind(std::bind(&EditorApp::on_asset_imported,
		this, std::placeholders::_1, std::placeholders::_2));
	initialize_asset_actions_mgr();
	initialize_asset_factory_mgr();

	/** Property editors */
	register_property_editor(reflection::Type::get<bool>(), new BoolPropertyEditor);

	/** Cookers */
	AssetCooker::initialize();

#if 0
	/** models */
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		ze::filesystem::FileIStream stream("Assets/landscapetonkh.obj", ze::filesystem::FileReadFlagBits::Binary);
		tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, &stream);

		robin_hood::unordered_map<ModelVertexCon, uint32_t> unique_vertices;

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				ModelVertexCon vertex;

				vertex.position.x = attrib.vertices[3 * index.vertex_index + 0];
				vertex.position.y = attrib.vertices[3 * index.vertex_index + 1];
				vertex.position.z = attrib.vertices[3 * index.vertex_index + 2];

				if (!attrib.texcoords.empty())
				{
					vertex.uv.x = attrib.texcoords[2 * index.texcoord_index + 0];
					vertex.uv.y = attrib.texcoords[2 * index.texcoord_index + 1];
				}

				if (!attrib.normals.empty())
				{
					vertex.normal.x = attrib.normals[3 * index.normal_index + 0];
					vertex.normal.y = attrib.normals[3 * index.normal_index + 1];
					vertex.normal.z = attrib.normals[3 * index.normal_index + 2];
				}

				if (unique_vertices.count(vertex) == 0)
				{
					unique_vertices[vertex] = static_cast<uint32_t>(landscape_vertices.size());
					landscape_vertices.push_back(vertex);
				}

				landscape_indices.push_back(unique_vertices[vertex]);
			}
		}

	}

	landscape_vbuffer = gfx::Device::get().create_buffer(gfx::BufferInfo::make_vertex_buffer(sizeof(ModelVertexCon) * landscape_vertices.size()),
		std::span((uint8_t*) landscape_vertices.data(), sizeof(ModelVertexCon) * landscape_vertices.size())).second;
	
	landscape_ibuffer = gfx::Device::get().create_buffer(gfx::BufferInfo::make_index_buffer(sizeof(uint32_t) * landscape_indices.size()),
		std::span((uint8_t*) landscape_indices.data(), sizeof(uint32_t) * landscape_indices.size())).second;

	landscape_playout = gfx::Device::get().create_pipeline_layout(gfx::PipelineLayoutCreateInfo(
		{ 
			gfx::DescriptorSetLayoutCreateInfo(
			{
				gfx::DescriptorSetLayoutBinding(0, gfx::DescriptorType::UniformBuffer, 1, gfx::ShaderStageFlagBits::Vertex),
			})
		})).second;

	std::string vertex_src = filesystem::read_file_to_string("Shaders/TonkhVS.hlsl");
	std::string fragment_src = filesystem::read_file_to_string("Shaders/TonkhFS.hlsl");

	using namespace ze::gfx;
	shaders::ShaderCompilerOutput vertex_output = shaders::compile_shader(gfx::shaders::ShaderStage::Vertex,
		"TonkhVS", 
		vertex_src,
		"Main",
		shaders::ShaderCompilerTarget::VulkanSpirV,
		false);

	shaders::ShaderCompilerOutput frag_output = shaders::compile_shader(gfx::shaders::ShaderStage::Fragment,
		"TonkhFS",
		fragment_src,
		"Main",
		shaders::ShaderCompilerTarget::VulkanSpirV,
		false);

	shader_vert = gfx::Device::get().create_shader(gfx::ShaderCreateInfo(vertex_output.bytecode)).second;
	shader_frag = gfx::Device::get().create_shader(gfx::ShaderCreateInfo(frag_output.bytecode)).second;

	depth_texture = gfx::Device::get().create_texture(gfx::TextureInfo::make_2d_texture(window->get_width(),
		window->get_height(),
		gfx::Format::D32SfloatS8Uint,
		1,
		TextureUsageFlagBits::DepthStencilAttachment)).second;

	depth_view = gfx::Device::get().create_texture_view(gfx::TextureViewInfo::make_2d_view(*depth_texture,
		gfx::Format::D32SfloatS8Uint,
		gfx::TextureSubresourceRange(gfx::TextureAspectFlagBits::Depth, 0, 1, 0, 1))).second;
#endif
}

EditorApp::~EditorApp()
{
	gfx::Device::get().wait_gpu_idle();
	destroy_asset_factory_mgr();
	destroy_asset_actions_mgr();
	ImGui::GetMainViewport()->RendererUserData = nullptr;
	main_viewport_data.window.swapchain.reset();
	ImGui::DestroyContext();
	ui::imgui::destroy();
	free_icons();
}

void EditorApp::process_event(const SDL_Event& in_event, const float in_delta_time)
{
	EngineApp::process_event(in_event, in_delta_time);

	if (in_event.type == SDL_WINDOWEVENT)
	{
		switch(in_event.window.event)
		{
		case SDL_WINDOWEVENT_CLOSE:
			if (SDL_GetWindowID(reinterpret_cast<SDL_Window*>(window->get_handle())) == in_event.window.windowID)
				exit(0);
			break;
		case SDL_WINDOWEVENT_RESIZED:
			int x = in_event.window.data1;
			int y = in_event.window.data2;
			using namespace ui::imgui;
			gfx::Device::get().wait_gpu_idle();
			ViewportData* data = reinterpret_cast<ViewportData*>(ImGui::GetMainViewport()->RendererUserData);
			gfx::Device::get().resize_swapchain(*data->window.swapchain,
				x, y);
			window->set_width(x);
			window->set_height(y);
			data->window.width = x;
			data->window.height = y;
			for (auto frame : data->window.has_rendered_one_frame)
			{
				frame = false;
			}
			break;
		}
	}

	maths::Vector3f fwd;
	fwd.x = cos(maths::radians(cam_yaw)) * cos(maths::radians(cam_pitch));
	fwd.y = sin(maths::radians(cam_yaw)) * cos(maths::radians(cam_pitch));
	fwd.z = sin(maths::radians(cam_pitch));

	if (in_event.type == SDL_MOUSEMOTION)
	{
		cam_yaw += ze::input::get_mouse_delta().x * 0.25f;
		cam_pitch -= ze::input::get_mouse_delta().y * 0.25f;

		if (cam_pitch > 89.0f)
			cam_pitch = 89.0f;

		if (cam_pitch < -89.0f)
			cam_pitch = -89.0f;

		maths::Vector3f fwd;
		fwd.x = cos(maths::radians(cam_yaw)) * cos(maths::radians(cam_pitch));
		fwd.y = sin(maths::radians(cam_yaw)) * cos(maths::radians(cam_pitch));
		fwd.z = sin(maths::radians(cam_pitch));
		cam_fwd = maths::normalize(fwd);
	}

	ImGui_ImplSDL2_ProcessEvent(&in_event);
}

void EditorApp::post_tick(const float in_delta_time)
{
	for(const auto& expired_child : expired_childs)
	{
		for(size_t i = 0; i < main_windows.size(); ++i)
		{
			if(main_windows[i].get() == expired_child)
			{
				main_windows.erase(main_windows.begin() + i);
				break;
			}
		}
	}
	expired_childs.clear();
	
	draw();
}

void EditorApp::draw()
{
	ImGui::SetCurrentContext(main_context);

	ImGui_ImplSDL2_NewFrame(reinterpret_cast<SDL_Window*>(window->get_handle()));
	ImGui::NewFrame();

	gfx::Device::get().new_frame();

	/** Process new windows to add */
	for (auto& window : main_windows_queue)
		main_windows.push_back(std::move(window));
	main_windows_queue.clear();

	/** UI rendering */
	ImGui::ShowDemoWindow();
	ImGui::ShowStyleEditor();
	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(window->get_width(), window->get_height()), ImGuiCond_Always);
	ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (ImGui::Begin("ZEEditor_MainWindow_BG", nullptr, ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoResize))
	{
		ImGui::DockSpace(ImGuiID(2201), ImVec2(window->get_width(), window->get_height()), 0);
	}
	ImGui::PopStyleVar(3);
	ImGui::End();

	for (const auto& window : main_windows)
	{
		bool expired = window->draw_window();
		if (!expired)
			expired_childs.emplace_back(window.get());
	}

	ImGui::Render();

	/** Update viewport buffers */
	ImGui::UpdatePlatformWindows();

	/** Main viewport */
	ui::imgui::imgui_render_window_callback(ImGui::GetMainViewport());
	ui::imgui::draw_viewports();
#if 0
	float camera_speed = 0.025f;

	if (input::is_key_held(SDL_SCANCODE_W))
	{
		cam_pos += cam_fwd * camera_speed * static_cast<float>(EngineApp::get_delta_time());
	}

	if (input::is_key_held(SDL_SCANCODE_S))
	{
		cam_pos -= cam_fwd * camera_speed * static_cast<float>(EngineApp::get_delta_time());
	}

	if (input::is_key_held(SDL_SCANCODE_A))
	{
		cam_pos += maths::normalize(maths::cross(cam_fwd, maths::Vector3f::get_up())) *
			camera_speed * static_cast<float>(EngineApp::get_delta_time());
	}

	if (input::is_key_held(SDL_SCANCODE_D))
	{
		cam_pos -= maths::normalize(maths::cross(cam_fwd, maths::Vector3f::get_up())) *
			camera_speed * static_cast<float>(EngineApp::get_delta_time());
	}

	using namespace gfx;
	using namespace ui::imgui;
	auto list = gfx::Device::get().allocate_cmd_list(CommandListType::Gfx);
	RenderPassInfo info;
	info.width = ImGui::GetMainViewport()->Size.x;
	info.height = ImGui::GetMainViewport()->Size.y;
	info.layers = 1;
	info.attachments = {
		gfx::AttachmentDescription(
			gfx::Format::B8G8R8A8Unorm,
			gfx::SampleCountFlagBits::Count1,
			gfx::AttachmentLoadOp::Clear,
			gfx::AttachmentStoreOp::Store,
			gfx::AttachmentLoadOp::DontCare,
			gfx::AttachmentStoreOp::DontCare,
			gfx::TextureLayout::Undefined,
			gfx::TextureLayout::Present),

		gfx::AttachmentDescription(
			gfx::Format::D32SfloatS8Uint,
			gfx::SampleCountFlagBits::Count1,
			gfx::AttachmentLoadOp::Clear,
			gfx::AttachmentStoreOp::Store,
			gfx::AttachmentLoadOp::DontCare,
			gfx::AttachmentStoreOp::DontCare,
			gfx::TextureLayout::Undefined,
			gfx::TextureLayout::DepthStencilAttachment),
	};
	info.subpasses = {
		gfx::SubpassDescription({},
			{
				gfx::AttachmentReference(0, gfx::TextureLayout::ColorAttachment) },
			{},
			{
				gfx::AttachmentReference(1, TextureLayout::DepthStencilAttachment)
			})
	};

	ViewportData* data = reinterpret_cast<ViewportData*>(ImGui::GetMainViewport()->RendererUserData);
	info.color_attachments[0] = gfx::Device::get().get_swapchain_backbuffer_texture_view(*data->window.swapchain);
	info.depth_attachments[0] = *depth_view;

	std::array<float, 4> float32 = { 0, 0, 0, 1 };
	ClearValue cv(float32);
	ClearValue depth(ClearDepthStencilValue(1.0f, 0));
	list->begin_render_pass(info, maths::Rect2D(0, 0, ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y), { cv, depth });
	list->bind_pipeline_layout(*landscape_playout);
	list->bind_vertex_buffer(*landscape_vbuffer, 0);
	list->bind_index_buffer(*landscape_ibuffer);
	list->bind_ubo(0, 0, ubo_wvp.get());

	maths::Matrix4f world;
	maths::Matrix4f view = maths::look_at(cam_pos,
		cam_pos + cam_fwd,
		maths::Vector3f::get_up());
	maths::Matrix4f proj = maths::infinite_perspective(maths::radians(90.f), 1.77777f, 0.1f);;

	TEEST teest;
	teest.wvp = proj * view * world;
	teest.world = world;
	teest.cam_pos = cam_pos;
	ubo_wvp.update(teest);

	GfxPipelineRenderPassState rp_state;
	rp_state.depth_stencil = gfx::PipelineDepthStencilStateCreateInfo(true,
		true,
		CompareOp::Less);

	GfxPipelineInstanceState i_state;
	i_state.vertex_input = PipelineVertexInputStateCreateInfo(
		{
			VertexInputBindingDescription(0, sizeof(ModelVertexCon), VertexInputRate::Vertex),
		},
		{
			VertexInputAttributeDescription(0, 0, Format::R32G32B32Sfloat, offsetof(ModelVertexCon, position)),
			VertexInputAttributeDescription(1, 0, Format::R32G32B32Sfloat, offsetof(ModelVertexCon, normal)),
			VertexInputAttributeDescription(2, 0, Format::R32G32Sfloat, offsetof(ModelVertexCon, uv)),
		});

	i_state.shaders = {
		gfx::GfxPipelineShaderStageInfo(gfx::ShaderStageFlagBits::Vertex,
			*shader_vert, "Main"),
		gfx::GfxPipelineShaderStageInfo(gfx::ShaderStageFlagBits::Fragment,
			*shader_frag, "Main"),
	};
	list->set_pipeline_render_pass_state(rp_state);
	list->set_pipeline_instance_state(i_state);
	list->set_viewport(gfx::Viewport(0, 0, ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y));
	list->set_scissor(maths::Rect2D(0, 0, ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y));
	list->draw_indexed(landscape_indices.size(), 1, 0, 0, 0);
	list->end_render_pass();
	gfx::Device::get().submit(list);
#endif
	gfx::Device::get().end_frame();

	ui::imgui::imgui_swap_buffers_callback(ImGui::GetMainViewport());
	ui::imgui::swap_viewports_buffers();
}

void EditorApp::draw_task()
{
	ImGui::SetCurrentContext(tasks_window_context);
	ImGui_ImplSDL2_NewFrame(reinterpret_cast<SDL_Window*>(window->get_handle()));
	ImGui::NewFrame();

	gfx::Device::get().new_frame();

	/** Task progress bar */
	// explicit viewport for task progress
	if (tasks.size() != 0)
	{
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, 
			ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		if (ImGui::Begin("ZEEditor_TaskWindow", nullptr, ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus))
		{
			for (size_t i = 0; i < std::min<size_t>(tasks.size(), 3); ++i)
			{
				ImGui::Text(tasks[i].text.c_str());
				ImGui::ProgressBar(static_cast<float>(tasks[i].completed_work) / tasks[i].work_amount);
			}
		}

		ImGui::End();
	}

	ImGui::Render();

	/** Update viewport buffers */
	ImGui::UpdatePlatformWindows();

	/** Main viewport */
	ui::imgui::imgui_render_window_callback(ImGui::GetMainViewport());
	ui::imgui::draw_viewports();

	gfx::Device::get().end_frame();

	ui::imgui::imgui_swap_buffers_callback(ImGui::GetMainViewport());
	ui::imgui::swap_viewports_buffers();
}

void EditorApp::on_asset_imported(const std::filesystem::path& in_path,
	const std::filesystem::path& in_target_path)
{	
	/** Try to find an asset factory compatible with this format */
	std::string extension = in_path.extension().string().substr(1, in_path.extension().string().size() - 1);
	AssetFactory* factory = get_factory_for_format(extension);
	if (!factory)
	{
		ze::logger::error("Asset {} can't be imported: unknown format", in_path.string());
		return;
	}

	/** Open a stream to the file */
	filesystem::FileIStream stream(in_path, filesystem::FileReadFlagBits::Binary);
	if (!stream)
	{
		ze::logger::error("Failed to open an input stream for file {}", in_path.string());
		return;
	}

	/** Serialize the asset and then unload it */
	OwnerPtr<Asset> asset = factory->create_from_stream(stream);
	const ze::reflection::Class* asset_class = asset->get_class();
	std::filesystem::path path = "Assets/";

	/** Compute path */
	{
		std::string name = in_path.stem().string();
		while (std::filesystem::exists(in_target_path / (name + "." + factory->get_asset_file_extension())))
			name += "(1)";
		name += "." + factory->get_asset_file_extension();
		path.concat(name);
		asset->set_path(path);
	}

	/** Set asset metadata file */
	{
		AssetMetadata metadata;
		metadata.uuid = uuids::uuid_system_generator{}();
		metadata.engine_version = get_version();
		metadata.asset_class = asset_class;
		asset->set_metadata(metadata);
	}

	logger::info("Imported {} as {} ({})", in_path.string(), 
		asset->get_path().stem().string(),
		asset->get_class()->get_name());

	assetmanager::save_asset(assetmanager::AssetSaveInfo(asset, get_current_platform()));
	delete asset;

	/** Notify the database */
	assetdatabase::scan(in_target_path);

	if (AssetActions* actions = get_actions_for(asset_class))
	{
		auto request = assetmanager::load_asset_sync(path);
		actions->open_editor(request.first, request.second);
	}
}

void EditorApp::add_window(OwnerPtr<Window> in_window) 
{ 
	main_windows_queue.emplace_back(std::unique_ptr<Window>(in_window)); 
}

bool EditorApp::has_window(const std::string& in_title)
{
	for(const auto& window : main_windows)
	{
		if(window->get_title() == in_title)
			return true;
	}

	return false;
}

}