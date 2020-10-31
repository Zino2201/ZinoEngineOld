#include "Editor/ZEEditor.h"
#include "Module/Module.h"
#include "Engine/Viewport.h"
#include "ImGui/ImGui.h"
#include "examples/imgui_impl_sdl.h"
#include "ImGui/ImGuiRenderer.h"
#include <SDL.h>
#include "Editor/Widgets/MapTabWidget.h"
#include "Assets/Asset.h"
#include "AssetDatabase/AssetDatabase.h"
#include <istream>
#include "Editor/Assets/AssetFactory.h"
#include "Assets/AssetManager.h"
#include "Editor/AssetUtils/AssetUtils.h"
#include "ZEFS/FileStream.h"
#include "Gfx/Backend.h"
#include "Engine/Viewport.h"
#include "Shader/ShaderCompiler.h"
#include <SDL.h>
#include "imgui_internal.h"
#include "Assets/Asset.h"

ZE_DEFINE_MODULE(ze::module::DefaultModule, ZEEditor);

namespace ze::editor
{

int event_filter(void* pthis, const SDL_Event *event)
{
    if (event->type == SDL_WINDOWEVENT &&
        event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
    {
        EditorApp* app = (EditorApp*) pthis;
		app->process_event(*event);
    }

    return 1;
}

EditorApp::EditorApp() : EngineApp(),
	main_window("ZinoEngine Editor", 1280, 720, 
		0, 0, 
		NativeWindowFlagBits::Centered | NativeWindowFlagBits::Resizable | NativeWindowFlagBits::Maximized)
{
	swapchain = gfx::RenderBackend::get().swapchain_create(
		ze::gfx::SwapChainCreateInfo(
			main_window.get_handle(),
			main_window.get_width(),
			main_window.get_height()));
	if(!swapchain)
		ze::logger::fatal("Failed to create swapchain");

	cmd_pool = gfx::RenderBackend::get().command_pool_create(gfx::CommandPoolType::Gfx);
	auto lists = gfx::RenderBackend::get().command_pool_allocate(*cmd_pool, 1);
	cmd_list = lists[0];

	cmd_list_fence = gfx::RenderBackend::get().fence_create(false);

	auto [render_pass_result, render_pass_handle] = gfx::RenderBackend::get().render_pass_create(
		gfx::RenderPassCreateInfo(
			{
				/** backbuffer */
				gfx::AttachmentDescription(
					gfx::Format::B8G8R8A8Unorm,
					gfx::SampleCountFlagBits::Count1,
					gfx::AttachmentLoadOp::Clear,
					gfx::AttachmentStoreOp::Store,
					gfx::AttachmentLoadOp::DontCare,
					gfx::AttachmentStoreOp::DontCare,
					gfx::TextureLayout::Undefined,
					gfx::TextureLayout::Present)
			},
			{
				gfx::SubpassDescription({}, { gfx::AttachmentReference(0, gfx::TextureLayout::ColorAttachment) })
			}));
	render_pass = render_pass_handle;

	SDL_SetEventFilter((SDL_EventFilter) &event_filter, this);

	initialize_asset_factory_mgr();

	assetutils::get_on_asset_imported().bind(std::bind(&EditorApp::on_asset_imported,
		this, std::placeholders::_1, std::placeholders::_2));

	/** Scan Assets directory */
	assetdatabase::scan("Assets", assetdatabase::AssetScanMode::Async);

	/** Initialize ImGui */
	ImGui_ImplSDL2_InitForVulkan(reinterpret_cast<SDL_Window*>(main_window.get_handle()));

	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	/** Default ZE editor style */
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 2.f;
		style.TabRounding = 2.f;
	}

	io.DisplaySize = ImVec2(static_cast<float>(main_window.get_width()), static_cast<float>(main_window.get_height()));
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	io.WantCaptureKeyboard = true;
	io.WantCaptureMouse = true;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	font = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto-Medium.ttf", 16.f);

	ze::ui::imgui::initialize(cmd_list, *render_pass);
}

EditorApp::~EditorApp()
{
	destroy_asset_factory_mgr();
	ze::ui::imgui::destroy();
}

void EditorApp::process_event(const SDL_Event& in_event)
{
	EngineApp::process_event(in_event);

	ImGuiIO& io = ImGui::GetIO();

	if(in_event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
	{
		gfx::RenderBackend::get().swapchain_resize(*swapchain,
			in_event.window.data1, in_event.window.data2);

		io.DisplaySize = ImVec2(in_event.window.data1, in_event.window.data2);
		main_window.set_width(in_event.window.data1);
		main_window.set_height(in_event.window.data2);

		post_tick(0);
	}

	ImGui_ImplSDL2_ProcessEvent(&in_event);
}

void EditorApp::post_tick(const float in_delta_time)
{
	ImGui_ImplSDL2_NewFrame(reinterpret_cast<SDL_Window*>(main_window.get_handle()));
	ImGui::NewFrame();
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(static_cast<float>(main_window.get_width()), static_cast<float>(main_window.get_height())),
		ImGuiCond_Always);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (ImGui::Begin("MainEditorWindow", nullptr, ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoResize))
	{
		ImGui::PopStyleVar(3);
		if (ImGui::BeginTabBar("MainTabBar", ImGuiTabBarFlags_TabListPopupButton
			| ImGuiTabBarFlags_Reorderable))
		{
			ImGui::SameLine(ImGui::GetColumnWidth() - 500);
			ImGui::Text("ZinoEngine v%d.%d.%d | FPS: %d | MS: %.2f",
				get_version().major, 
				get_version().minor,
				get_version().patch,
				static_cast<int32_t>(1.0 / in_delta_time), 
				in_delta_time * 1000.0);
			draw_main_tab();
			ImGui::EndTabBar();
		}
	}
	ImGui::End();
	ImGui::Render();

	using namespace gfx;

	RenderBackend::get().new_frame();

	int w = 0, h = 0;
	SDL_GetWindowSize(reinterpret_cast<SDL_Window*>(main_window.get_handle()), &w, &h);
	if((SDL_GetWindowFlags(reinterpret_cast<SDL_Window*>(main_window.get_handle())) 
		& SDL_WINDOW_MINIMIZED) || w == 0 || h == 0)
		return;

	ze::ui::imgui::update();

	/** Draw */
	if(RenderBackend::get().swapchain_acquire_image(*swapchain))
	{
		RenderBackend::get().command_pool_reset(*cmd_pool);
		RenderBackend::get().command_list_begin(cmd_list);

		gfx::Framebuffer fb;
		fb.color_attachments[0] = RenderBackend::get().swapchain_get_backbuffer(*swapchain);
		fb.width = w;
		fb.height = h;
		fb.layers = 1;
		RenderBackend::get().cmd_begin_render_pass(
			cmd_list,
			*render_pass,
			fb,
			maths::Rect2D(maths::Vector2f(),
				maths::Vector2f(w, h)),
			{
				gfx::ClearColorValue({0, 0, 0, 1})
			});
		RenderBackend::get().cmd_set_viewport(cmd_list, 0, { gfx::Viewport(0, 0, w, h) });
		ze::ui::imgui::draw(cmd_list);
		RenderBackend::get().cmd_end_render_pass(cmd_list);
		RenderBackend::get().command_list_end(cmd_list);
		RenderBackend::get().queue_execute(
			RenderBackend::get().get_gfx_queue(),
			{ cmd_list },
			*cmd_list_fence);
		RenderBackend::get().swapchain_present(*swapchain);

		// TODO: MOVE UP start signaled
		RenderBackend::get().fence_wait_for({ *cmd_list_fence });
		RenderBackend::get().fence_reset({ *cmd_list_fence });

	}
}

void EditorApp::draw_main_tab()
{
	/** Main tab */
	if (!ImGui::BeginTabItem("Main"))
		return;

	static CMapTabWidget wid;
	wid.Draw();

	ImGui::EndTabItem();
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
	assetutils::save_asset(*asset, in_target_path,
		"NewAssetTest");
	delete asset;

	/** Notify the database */
	assetdatabase::scan(in_target_path);
}

}