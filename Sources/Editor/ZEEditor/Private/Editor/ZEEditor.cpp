#include "Editor/ZEEditor.h"
#include "Module/Module.h"
#include "Engine/Viewport.h"
#include "ImGui/ImGui.h"
#include "backends/imgui_impl_sdl.h"
#include <SDL.h>
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
		0, 0, NativeWindowFlagBits::Centered | NativeWindowFlagBits::Resizable | NativeWindowFlagBits::Maximized))
{
	app = this;

	assetdatabase::scan("Assets", assetdatabase::AssetScanMode::Sync);

	/** Initialize ImGui */
	ImGui::SetCurrentContext(ImGui::CreateContext());

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
		colors[ImGuiCol_FrameBgActive]          = ImVec4(0.00f, 0.48f, 1.00f, 0.67f);
		colors[ImGuiCol_TitleBg]                = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		colors[ImGuiCol_TitleBgActive]          = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark]              = ImVec4(0.71f, 0.71f, 0.71f, 1.00f);
		colors[ImGuiCol_SliderGrab]             = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
		colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
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
	ze::ui::imgui::initialize();
	ImGui_ImplSDL2_InitForVulkan(reinterpret_cast<SDL_Window*>(window->get_handle()));

	/** Create gfx resources */
	ImGui::GetMainViewport()->RendererUserData = &main_viewport_data;
	main_viewport_data.window.swapchain = gfx::Device::get().create_swapchain(gfx::SwapChainCreateInfo(window->get_handle(),
		window->get_width(), window->get_height())).second;
	main_viewport_data.window.width = window->get_width();
	main_viewport_data.window.height = window->get_height();
	main_viewport_data.owned_by_renderer = false;

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
			break;
		}
	}

	ImGui_ImplSDL2_ProcessEvent(&in_event);
}

void EditorApp::post_tick(const float in_delta_time)
{
	ImGui_ImplSDL2_NewFrame(reinterpret_cast<SDL_Window*>(window->get_handle()));
	ImGui::NewFrame();
	using namespace gfx;

	Device::get().new_frame();

	/** Process new windows to add */
	for(auto& window : main_windows_queue)
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
	if(ImGui::Begin("ZEEditor_MainWindow_BG", nullptr, ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoResize))
	{
		ImGui::DockSpace(ImGuiID(2201), ImVec2(window->get_width(), window->get_height()), 0);
	}
	ImGui::PopStyleVar(3);
	ImGui::End();

	std::vector<Window*> expired_childs;
	for(const auto& window : main_windows)
	{
		bool expired = window->draw_window();
		if(!expired)
			expired_childs.emplace_back(window.get());
	}

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

	ImGui::Render();

	/** Update viewport buffers */
	ImGui::UpdatePlatformWindows();

	/** Main viewport */
	ui::imgui::imgui_render_window_callback(ImGui::GetMainViewport());
	ui::imgui::draw_viewports();

	Device::get().end_frame();

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
	Asset* asset = factory->create_from_stream(stream);
	const ze::reflection::Class* asset_class = asset->get_class();
	std::string name = in_path.filename().replace_extension("").string();
	while(std::filesystem::exists(in_target_path / (name + ".zeasset")))
		name += "(1)";

	assetutils::save_asset(*asset, in_target_path, name);
	delete asset;

	/** Notify the database */
	assetdatabase::scan(in_target_path);

	if(AssetActions* actions = get_actions_for(asset_class))
	{
		auto request = assetmanager::load_asset_sync(in_target_path / (name + ".zeasset"));
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