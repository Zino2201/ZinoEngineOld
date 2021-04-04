#include "Editor/ZEEditor.h"
#include "Module/Module.h"
#include "Engine/Viewport.h"
#include "ImGui/ImGui.h"
#include "examples/imgui_impl_sdl.h"
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
		0, 0, NativeWindowFlagBits::Centered)),
	map_editor(std::make_unique<MapEditor>())
{
	app = this;

	assetdatabase::scan("Assets", assetdatabase::AssetScanMode::Sync);

	/** Initialize ImGui */
	ImGui::SetCurrentContext(ImGui::CreateContext());

	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	/** Default ZE editor style */
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 2.f;
		style.TabRounding = 2.f;
	}

	io.DisplaySize = ImVec2(static_cast<float>(window->get_width()), static_cast<float>(window->get_height()));
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	io.WantCaptureKeyboard = true;
	io.WantCaptureMouse = true;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

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
}

EditorApp::~EditorApp()
{
	gfx::Device::get().wait_gpu_idle();
	ImGui::GetMainViewport()->RendererUserData = nullptr;
	main_viewport_data.window.swapchain.reset();
	ImGui::DestroyContext();
	ui::imgui::destroy();
}

void EditorApp::process_event(const SDL_Event& in_event, const float in_delta_time)
{
	EngineApp::process_event(in_event, in_delta_time);

	if (in_event.type == SDL_WINDOWEVENT
		&& in_event.window.event == SDL_WINDOWEVENT_CLOSE)
	{
		if (SDL_GetWindowID(reinterpret_cast<SDL_Window*>(window->get_handle())) == in_event.window.windowID)
        {
			exit(0);
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

	/** UI rendering */
	ImGui::ShowDemoWindow();
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(window->get_width(), window->get_height()));
	ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
	ImGui::Begin("ZEEditor_MainWindow_BG", nullptr, ImGuiWindowFlags_NoTitleBar 
		| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking | ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	map_editor->draw_window();
	ImGui::End();

	ImGui::Render();

	/** Draw imgui windows */
	ImGui::UpdatePlatformWindows();

	/** Main viewport */
	ui::imgui::imgui_render_window_callback(ImGui::GetMainViewport());
	ui::imgui::draw_viewports();

	Device::get().end_frame();

	/** Don't present if we didn't submit things (or else we will deadlock the gpu) */
	ui::imgui::imgui_swap_buffers_callback(ImGui::GetMainViewport());
	ui::imgui::swap_viewports_buffers();
}

void EditorApp::on_asset_imported(const std::filesystem::path& in_path,
	const std::filesystem::path& in_target_path)
{

}

}