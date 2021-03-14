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
#include "Gfx/Gfx.h"
#include "Engine/Viewport.h"
#include "Shader/ShaderCompiler.h"
#include <SDL.h>
#include "imgui_internal.h"
#include "Assets/Asset.h"
#include "Engine/Components/TransformComponent.h"
#include "Engine/Components/RelationshipComponent.h"
#include "Engine/World.h"
#include "Engine/ECS/EntityManager.h"
#include "Editor/ArithmeticPropertyEditors.h"
#include "Editor/VectorPropertyEditor.h"
#include "Editor/Widgets/MapTabWidget.h"
#include "Maths/Matrix.h"
#include "Maths/Matrix/Transformations.h"
#include "Editor/Assets/AssetActions.h"
#include "Editor/Widgets/Tab.h"
#include "Editor/NotificationSystem.h"
#include <sstream>
#include "Gfx/GpuVector.h"
#include "ZEFS/ZEFS.h"
#include "ZEUI/Primitives/Text.h"
#include "ZEUI/Primitives/DockSpace.h"
#include "ZEUI/Primitives/DockableTab.h"
#include "ZEUI/Primitives/DockTabBar.h"
#include "Editor/Widgets/MapEditorTab.h"
#include "Editor/Widgets/AssetExplorer.h"

ZE_DEFINE_MODULE(ze::module::DefaultModule, ZEEditor);

namespace ze::editor
{

int event_filter(void* pthis, const SDL_Event *event)
{
    if (event->type == SDL_WINDOWEVENT &&
        event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
    {
        EditorApp* app = (EditorApp*) pthis;
		app->process_event(*event, 0);
    }

    return 1;
}

EditorApp* app = nullptr;

EditorApp& EditorApp::get()
{
	return *app;
}

EditorApp::EditorApp() : EngineApp()
{
	app = this;

	window = make_widget_unique<ui::Window>(1280, 720, "ZinoEngine Editor (ZEUI)", 0, 0, 
		ui::WindowFlagBits::Centered | ui::WindowFlagBits::Resizable | ui::WindowFlagBits::Maximized);
	
	swapchain = gfx::Device::get().create_swapchain(
		ze::gfx::SwapChainCreateInfo(
			window->get_handle(),
			window->get_width(),
			window->get_height())).second;
	if(!swapchain)
		ze::logger::fatal("Failed to create swapchain");


	/** Initialize ImGui */
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForVulkan(reinterpret_cast<SDL_Window*>(window->get_handle()));

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
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	font = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto-Medium.ttf", 18.f);
	ze::ui::imgui::initialize();
	ImGui::SetCurrentFont(font);
	ImGui::NewFrame();
	ImGui::Render();

	ui_renderer = std::make_unique<ui::Renderer>();

	{
		using namespace ui;

		window->content()
		[
			make_widget<AssetExplorer>()
			/*make_widget<DockTabBar>()
			+ make_item<DockTabBar>()
			->content()
			[
				make_widget<MapEditorTab>()
			]
			+ make_item<DockTabBar>()
			->content()
			[
				make_widget<AssetExplorer>()
			]*/
		];
	}

	SDL_SetEventFilter((SDL_EventFilter) &event_filter, this);

	initialize_asset_factory_mgr();
	initialize_asset_actions_mgr();
	
	assetutils::get_on_asset_imported().bind(std::bind(&EditorApp::on_asset_imported,
		this, std::placeholders::_1, std::placeholders::_2));

	/** Scan Assets directory */
	assetdatabase::scan("Assets", assetdatabase::AssetScanMode::Async);

	world = std::make_unique<World>();

	size_t c = 147;
	size_t x = 0;
	size_t y = 0;
	for(size_t i = 0; i < c; ++i)
	{
		x++;

		if(x > c / 3)
		{
			x = 0;
			y += 1;
		}

		Entity ent = world->spawn_entity();
		TransformComponent& data = world->add_component<TransformComponent>(ent);
		data.position.x = x - 0.5;
		data.position.y = y;
	}

	map_tab_widget = std::make_unique<CMapTabWidget>(*world.get());

	/*** type editors */
	register_property_editor(reflection::Type::get<bool>(), new BoolPropertyEditor);
	register_property_editor(reflection::Type::get<uint32_t>(), new Uint32PropertyEditor);
	register_property_editor(reflection::Type::get<float>(), new FloatPropertyEditor);
	register_property_editor(reflection::Type::get<maths::Vector3f>(), new Vector3fPropertyEditor);
	register_property_editor(reflection::Type::get<maths::Vector3d>(), new Vector3dPropertyEditor);
}

EditorApp::~EditorApp()
{
	gfx::Device::get().wait_gpu_idle();
	assetmanager::unload_all();
	destroy_asset_actions_mgr();
	destroy_asset_factory_mgr();
	ze::ui::imgui::destroy();
	ImGui::DestroyContext();
}

void EditorApp::add_tab(OwnerPtr<Tab> in_tab)
{
	tabs.emplace_back(std::unique_ptr<Tab>(in_tab));
}

void EditorApp::process_event(const SDL_Event& in_event, const float in_delta_time)
{
	EngineApp::process_event(in_event, in_delta_time);

	ImGuiIO& io = ImGui::GetIO();

	if(in_event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
	{
		gfx::Device::get().wait_gpu_idle();

		gfx::Device::get().resize_swapchain(*swapchain,
			in_event.window.data1, in_event.window.data2);

		io.DisplaySize = ImVec2(in_event.window.data1, in_event.window.data2);
		window->set_width(in_event.window.data1);
		window->set_height(in_event.window.data2);
	}

	ImGui_ImplSDL2_ProcessEvent(&in_event);
	map_tab_widget->get_map_editor().process_event(in_event, in_delta_time);
}

void EditorApp::post_tick(const float in_delta_time)
{
    notifications_update(in_delta_time);

#if 0
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
		ImGui::ShowDemoWindow();

		/** Notifications */
        const float width = 340;
        const float height = 80;
        const float offset_x = 50;
        const float offset_y = 15;
        float win_offset_y = height + offset_y;
        for(const auto& notification : notifications_get())
        {
            ImGuiIO& io = ImGui::GetIO();

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 0.0f));
            ImGui::SetNextWindowSize(ImVec2(width, height));
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - (width + offset_x), io.DisplaySize.y - win_offset_y));
            ImGui::Begin(notification.message.c_str(), nullptr, ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::SetCursorPosY((height / 2) - (ImGui::CalcTextSize(notification.message.c_str(), nullptr, false, width).y) / 2);
            ImGui::TextWrapped("%s", notification.message.c_str());
            win_offset_y += ImGui::GetWindowSize().y + offset_y;
            ImGui::End();
            ImGui::PopStyleVar(1);
        }

		if (ImGui::BeginTabBar("MainTabBar", ImGuiTabBarFlags_TabListPopupButton
			| ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs))
		{
			ImGui::SameLine(ImGui::GetColumnWidth() - 500);
			ImGui::Text("ZinoEngine v%d.%d.%d | FPS: %d | MS: %.2f | %s",
				get_version().major, 
				get_version().minor,
				get_version().patch,
				static_cast<int32_t>(1.0 / in_delta_time), 
				in_delta_time * 1000.0,
				ZE_CONFIGURATION_NAME);
			draw_main_tab();

			size_t tab_to_remove = -1;

			size_t i = 0;
			for(const auto& tab : tabs)
			{
				bool open = true;
				bool draw = ImGui::BeginTabItem(tab->get_name().c_str(), &open);
				if(!open)
					tab_to_remove = i;
				i++;
				if(draw)
				{
					tab->draw();
					ImGui::EndTabItem();
				}
			}

			if(tab_to_remove != -1)
			{
				tabs.erase(tabs.begin() + tab_to_remove);
			}

			ImGui::EndTabBar();
		}
	}
	ImGui::End();

#endif

	ImGui_ImplSDL2_NewFrame(reinterpret_cast<SDL_Window*>(window->get_handle()));
	ImGui::NewFrame();
	ImGui::ShowDemoWindow();

	using namespace gfx;

	Device::get().new_frame();
	ui_renderer->new_frame();

	int w = 0, h = 0;
	SDL_GetWindowSize(reinterpret_cast<SDL_Window*>(window->get_handle()), &w, &h);
	if((SDL_GetWindowFlags(reinterpret_cast<SDL_Window*>(window->get_handle())) 
		& SDL_WINDOW_MINIMIZED) || w == 0 || h == 0)
		return;

	/** Draw */
	if(Device::get().acquire_swapchain_texture(*swapchain))
	{
		using namespace gfx;

		CommandList* list = Device::get().allocate_cmd_list(CommandListType::Gfx);

		gfx::RenderPassInfo render_pass;
		render_pass.attachments = {
			gfx::AttachmentDescription(
				gfx::Format::B8G8R8A8Unorm,
				gfx::SampleCountFlagBits::Count1,
				gfx::AttachmentLoadOp::Clear,
				gfx::AttachmentStoreOp::Store,
				gfx::AttachmentLoadOp::DontCare,
				gfx::AttachmentStoreOp::DontCare,
				gfx::TextureLayout::Undefined,
				gfx::TextureLayout::Present),
		};
		render_pass.subpasses = {
			gfx::SubpassDescription({}, { gfx::AttachmentReference(0, gfx::TextureLayout::ColorAttachment) })
		};
		render_pass.color_attachments[0] = Device::get().get_swapchain_backbuffer_texture_view(*swapchain);
		render_pass.width = w;
		render_pass.height = h;
		render_pass.layers = 1;
		list->begin_render_pass(render_pass,
			maths::Rect2D(maths::Vector2f(),
				maths::Vector2f(w, h)),
			{
				gfx::ClearColorValue({0, 0, 0, 1})
			});

		list->set_viewport(gfx::Viewport(0, 0, w, h));
		list->set_scissor(maths::Rect2D({ 0, 0 }, { w, h }));

		ui_renderer->render(*window.get(), list);

		ImGui::Render();
		ze::ui::imgui::update();
		ze::ui::imgui::draw(list);

		list->end_render_pass();
		Device::get().submit(list);
	}

	Device::get().end_frame();
	Device::get().present(*swapchain);
}

bool EditorApp::has_tab(std::string in_name)
{
	for(const auto& tab : tabs)
		if(tab->get_name() == in_name)
			return true;

	return false;
}

void EditorApp::test_renderer(const gfx::ResourceHandle& in_cmd_list)
{

}

void EditorApp::draw_main_tab()
{
	/** Main tab */
	if (!ImGui::BeginTabItem("Main"))
		return;

	map_tab_widget->Draw();

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
	const ze::reflection::Class* asset_class = asset->get_class();
	std::string name = "NewAsset";
	while(filesystem::exists(in_target_path / (name + ".zasset")))
		name += "(1)";

	assetutils::save_asset(*asset, in_target_path, name);
	delete asset;

	/** Notify the database */
	assetdatabase::scan(in_target_path);

	if(AssetActions* actions = get_actions_for(asset_class))
	{
		auto request = assetmanager::load_asset_sync(in_target_path / (name + ".zasset"));
		actions->open_editor(request.first, request.second);
	}
}

}