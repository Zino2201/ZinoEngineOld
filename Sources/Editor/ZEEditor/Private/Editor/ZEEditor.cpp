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
#include "Gfx/CommandSystem.h"
#include "Editor/NotificationSystem.h"
#include <sstream>
#include "Gfx/GpuVector.h"
#include "ZEFS/ZEFS.h"

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

EditorApp::EditorApp() : EngineApp(),
	main_window("ZinoEngine Editor", 1280, 720, 
		0, 0, 
		NativeWindowFlagBits::Centered | NativeWindowFlagBits::Resizable | NativeWindowFlagBits::Maximized)
{
	app = this;

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
					gfx::TextureLayout::Present),
			},
			{
				gfx::SubpassDescription({}, { gfx::AttachmentReference(0, gfx::TextureLayout::ColorAttachment) }),
			}));
	render_pass = render_pass_handle;

	auto [vp_render_pass_result, vp_render_pass_handle] = gfx::RenderBackend::get().render_pass_create(
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
					gfx::TextureLayout::ShaderReadOnly),
			},
			{
				gfx::SubpassDescription({}, { gfx::AttachmentReference(0, gfx::TextureLayout::ColorAttachment) }),
			}));
	vp_render_pass = vp_render_pass_handle;

	SDL_SetEventFilter((SDL_EventFilter) &event_filter, this);

	initialize_asset_factory_mgr();
	initialize_asset_actions_mgr();
	
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

	font = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto-Medium.ttf", 18.f);

	ze::ui::imgui::initialize(cmd_list, *render_pass);

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

	/** TEST VP */

	auto [layout_result, pipeline_layout_handle] = gfx::RenderBackend::get().pipeline_layout_create(
		gfx::PipelineLayoutCreateInfo(
			{ },
			{
				gfx::PushConstantRange(gfx::ShaderStageFlagBits::Vertex,
					0,
					sizeof(glm::mat4))
			}));
	vp_pipeline_layout = pipeline_layout_handle;

	/** Compile shaders */
		gfx::shaders::ShaderCompilerOutput vs_ = gfx::shaders::compile_shader(
			gfx::shaders::ShaderStage::Vertex,
			"Tests/TriangleVS.hlsl",
			"Main",
			gfx::shaders::ShaderCompilerTarget::VulkanSpirV,
			true).get();

		gfx::shaders::ShaderCompilerOutput fs_ = gfx::shaders::compile_shader(
			gfx::shaders::ShaderStage::Fragment,
			"Tests/TriangleFS.hlsl",
			"Main",
			gfx::shaders::ShaderCompilerTarget::VulkanSpirV,
			true).get();

		vs = gfx::RenderBackend::get().shader_create(gfx::ShaderCreateInfo(
			vs_.bytecode)).second;

		fs = gfx::RenderBackend::get().shader_create(gfx::ShaderCreateInfo(
			fs_.bytecode)).second;

	auto [pipeline_result, pipeline_handle] = gfx::RenderBackend::get().gfx_pipeline_create(
		gfx::GfxPipelineCreateInfo(
			{
				gfx::GfxPipelineShaderStage(
					gfx::ShaderStageFlagBits::Vertex,
					*vs,
					"Main"),
				gfx::GfxPipelineShaderStage(
					gfx::ShaderStageFlagBits::Fragment,
					*fs,
					"Main"),
			},
			gfx::PipelineVertexInputStateCreateInfo(),
			gfx::PipelineInputAssemblyStateCreateInfo(),
			gfx::PipelineRasterizationStateCreateInfo(),
			gfx::PipelineMultisamplingStateCreateInfo(),
			gfx::PipelineDepthStencilStateCreateInfo(),
			gfx::PipelineColorBlendStateCreationInfo(
				false, gfx::LogicOp::NoOp,
				{
					gfx::PipelineColorBlendAttachmentState(false)
				}),
			*vp_pipeline_layout,
			*vp_render_pass));
	vp_pipeline = pipeline_handle;

	gfx::hlcs::initialize();
}

EditorApp::~EditorApp()
{
	gfx::hlcs::destroy();
	assetmanager::unload_all();
	destroy_asset_actions_mgr();
	destroy_asset_factory_mgr();
	ze::ui::imgui::destroy();
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
		gfx::RenderBackend::get().swapchain_resize(*swapchain,
			in_event.window.data1, in_event.window.data2);

		io.DisplaySize = ImVec2(in_event.window.data1, in_event.window.data2);
		main_window.set_width(in_event.window.data1);
		main_window.set_height(in_event.window.data2);

		post_tick(in_delta_time);
	}

	ImGui_ImplSDL2_ProcessEvent(&in_event);
	map_tab_widget->get_map_editor().process_event(in_event, in_delta_time);
}

void EditorApp::post_tick(const float in_delta_time)
{
    notifications_update(in_delta_time);

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
	ImGui::Render();

	using namespace gfx;

	RenderBackend::get().new_frame();
	gfx::hlcs::begin_new_frame();

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

		/** MAIN VIEWPORT */
		gfx::Framebuffer vp_fb;
		vp_fb.color_attachments[0] = map_tab_widget->get_map_editor().get_viewport().get_color_attachment_view();
		vp_fb.width = map_tab_widget->get_map_editor().get_viewport().get_viewport().width;
		vp_fb.height = map_tab_widget->get_map_editor().get_viewport().get_viewport().height;
		vp_fb.layers = 1;
		RenderBackend::get().cmd_begin_render_pass(
			cmd_list,
			*vp_render_pass,
			vp_fb,
			maths::Rect2D(maths::Vector2f(),
				maths::Vector2f(map_tab_widget->get_map_editor().get_viewport().get_viewport().width, 
					map_tab_widget->get_map_editor().get_viewport().get_viewport().height)),
			{
				gfx::ClearColorValue({0, 0, 0, 1})
			});
		RenderBackend::get().cmd_set_viewport(cmd_list, 0, { map_tab_widget->get_map_editor().get_viewport().get_viewport() });
		RenderBackend::get().cmd_set_scissor(cmd_list, 0, { maths::Rect2D(maths::Vector2f(0), 
			maths::Vector2f(map_tab_widget->get_map_editor().get_viewport().get_viewport().width,
				map_tab_widget->get_map_editor().get_viewport().get_viewport().height)) });
		test_renderer(cmd_list);
		RenderBackend::get().cmd_end_render_pass(cmd_list);
		
		/** EDITOR */
		if(map_tab_widget->get_map_editor().is_viewport_fullscreen())
		{
			RenderBackend::get().cmd_pipeline_barrier(cmd_list,
				gfx::PipelineStageFlagBits::ColorAttachmentOutput,
				gfx::PipelineStageFlagBits::Transfer,
				{
					gfx::TextureMemoryBarrier(map_tab_widget->get_map_editor().get_viewport().get_texture(),
						gfx::AccessFlagBits::ColorAttachmentWrite,
						gfx::AccessFlagBits::TransferRead,
						gfx::TextureLayout::ShaderReadOnly,
						gfx::TextureLayout::TransferSrc,
						gfx::TextureSubresourceRange(gfx::TextureAspectFlagBits::Color, 0, 1, 0, 1)),
					
					gfx::TextureMemoryBarrier(RenderBackend::get().swapchain_get_backbuffer_texture(*swapchain),
						gfx::AccessFlags(),
						gfx::AccessFlagBits::TransferWrite,
						gfx::TextureLayout::Undefined,
						gfx::TextureLayout::TransferDst,
						gfx::TextureSubresourceRange(gfx::TextureAspectFlagBits::Color, 0, 1, 0, 1)),
				});
			
			RenderBackend::get().cmd_copy_texture(cmd_list,
				map_tab_widget->get_map_editor().get_viewport().get_texture(),
				gfx::TextureLayout::TransferSrc,
				RenderBackend::get().swapchain_get_backbuffer_texture(*swapchain),
				gfx::TextureLayout::TransferDst,
				{
					gfx::TextureCopyRegion(
						gfx::TextureSubresourceLayers(gfx::TextureAspectFlagBits::Color, 0, 0, 1),
						gfx::Offset3D(),
						gfx::TextureSubresourceLayers(gfx::TextureAspectFlagBits::Color, 0, 0, 1),
						gfx::Offset3D(),
						gfx::Extent3D(map_tab_widget->get_map_editor().get_viewport().get_viewport().width,
							map_tab_widget->get_map_editor().get_viewport().get_viewport().height, 
							1))
				});

			RenderBackend::get().cmd_pipeline_barrier(cmd_list,
				gfx::PipelineStageFlagBits::Transfer,
				gfx::PipelineStageFlagBits::Transfer,
				{
					gfx::TextureMemoryBarrier(RenderBackend::get().swapchain_get_backbuffer_texture(*swapchain),
						gfx::AccessFlagBits::TransferWrite,
						gfx::AccessFlags(),
						gfx::TextureLayout::TransferDst,
						gfx::TextureLayout::Present,
						gfx::TextureSubresourceRange(gfx::TextureAspectFlagBits::Color, 0, 1, 0, 1))
				});
		}
		else
		{
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
		}
		RenderBackend::get().command_list_end(cmd_list);
	}

	gfx::hlcs::submit_all_lists();

	RenderBackend::get().queue_execute(
		RenderBackend::get().get_gfx_queue(),
		{ cmd_list },
		*cmd_list_fence);
	RenderBackend::get().swapchain_present(*swapchain);

	// TODO: MOVE UP start signaled
	RenderBackend::get().fence_wait_for({ *cmd_list_fence });
	RenderBackend::get().fence_reset({ *cmd_list_fence });
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
	gfx::RenderBackend::get().cmd_bind_pipeline(
		in_cmd_list,
		gfx::PipelineBindPoint::Gfx,
		*vp_pipeline);

	for (Entity entity : world->get_entity_mgr().get_entities())
	{
		if(!world->get_component_mgr().has_component(entity, reflection::Class::get<TransformComponent>()))
			continue;

		auto& transform = world->get_component_mgr().get_component<TransformComponent>(entity);
		
		maths::Matrix4f world = maths::translate(
			maths::Vector3f(static_cast<float>(transform.position.x),
				static_cast<float>(transform.position.y),
				static_cast<float>(transform.position.z))) * 
			maths::scale(transform.scale);

		maths::Vector3f cam_pos(static_cast<float>(map_tab_widget->get_map_editor().get_cam_pos().x),
				static_cast<float>(map_tab_widget->get_map_editor().get_cam_pos().y),
				static_cast<float>(map_tab_widget->get_map_editor().get_cam_pos().z));

		maths::Matrix4f view = maths::look_at(cam_pos,
			cam_pos + map_tab_widget->get_map_editor().get_cam_fwd(),
			maths::Vector3f::get_up());

		maths::Matrix4f proj = maths::infinite_perspective(maths::radians(90.f), 
			map_tab_widget->get_map_editor().get_viewport().get_viewport().width / 
			(float) map_tab_widget->get_map_editor().get_viewport().get_viewport().height, 0.1f);

		maths::Matrix4f wvp = proj * view * world;

		gfx::RenderBackend::get().cmd_push_constants(in_cmd_list,
			*vp_pipeline_layout,
			gfx::ShaderStageFlagBits::Vertex,
			0,
			sizeof(maths::Matrix4f),
			&wvp);

		gfx::RenderBackend::get().cmd_draw(in_cmd_list, 3, 1, 0, 0);
	}
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