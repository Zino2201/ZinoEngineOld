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
#include "Editor/Widgets/ConvarViewer.h"
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
#include "ZEUI/Primitives/Box.h"
#include "ZEUI/Primitives/DockTabBar.h"
#include "ZEUI/Containers/VerticalContainer.h"
#include "Editor/Widgets/MapEditorTab.h"
#include "Editor/Widgets/AssetExplorer.h"
#include "ZEFS/Utils.h"
#include "SDFFontGen.h"
#include "ZEUI/Primitives/Text.h"
#include "ZEUI/Containers/Docking.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QFontDatabase>
#include <QFileInfo>
#include <QLabel>
#include <QDateTime>
#include <QProxyStyle>
#include "Editor/MainWindow.h"
#include "Editor/Windows/AssetExplorer.h"
#include "Editor/Windows/EntityProperties.h"
#include "Editor/Windows/EntityList.h"
#include "Editor/Windows/Viewport.h"

ZE_DEFINE_MODULE(ze::module::DefaultModule, ZEEditor);

namespace ze::editor
{

class ZEProxyStyle : public QProxyStyle
{
public:
	using QProxyStyle::QProxyStyle;

	int styleHint(StyleHint hint, const QStyleOption* option, 
		const QWidget* widget, QStyleHintReturn* returnData) const override
    {
        if (hint == QStyle::SH_ToolTip_WakeUpDelay)
        {
            return 0;
        }

        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
};

EditorApp* app = nullptr;

EditorApp& EditorApp::get()
{
	return *app;
}

EditorApp::EditorApp() : EngineApp()
{
	app = this;

	assetdatabase::scan("Assets", assetdatabase::AssetScanMode::Sync);

	int argc = 0;
	char* argv = nullptr;
	qt_app = std::make_unique<QApplication>(argc, &argv);
	qt_app->setStyle(new ZEProxyStyle(qt_app->style()));

	QFontDatabase::addApplicationFont(":Fonts/Roboto-Bold.ttf");

	QFile f(":Style.qss");
	if (f.exists())   
	{
		f.open(QFile::ReadOnly | QFile::Text);
		QTextStream ts(&f);
		qt_app->setStyleSheet(ts.readAll());
	}

	main_window = std::make_unique<MainWindow>();
	main_window->showMaximized();
	main_window->add_window(new AssetExplorer);
	main_window->add_window(new EntityProperties);
	main_window->add_window(new EntityList);
	main_window->add_window(new Viewport);
}

EditorApp::~EditorApp()
{
	gfx::Device::get().wait_gpu_idle();
}

void EditorApp::add_tab(OwnerPtr<Tab> in_tab)
{
	tabs.emplace_back(std::unique_ptr<Tab>(in_tab));
}

void EditorApp::process_event(const SDL_Event& in_event, const float in_delta_time)
{
	EngineApp::process_event(in_event, in_delta_time);
}

void EditorApp::post_tick(const float in_delta_time)
{
	qt_app->processEvents();

#if ZE_DEBUG
	QFile f("C:\\Projects\\ZinoEngine\\Sources\\Editor\\ZEEditor\\Assets\\Style.qss");
	static QDateTime last_write_time;
	QFileInfo info(f);

	if (f.exists() && info.lastModified() != last_write_time)   
	{
		f.open(QFile::ReadOnly | QFile::Text);
		QTextStream ts(&f);
		qt_app->setStyleSheet(ts.readAll());
	}

	last_write_time = info.lastModified();
#endif

	main_window->get_perf_text()->setText(QString::asprintf("FPS: %i (%.2f ms)",
		(int) (1.f / in_delta_time), in_delta_time * 1000.f));

#if 0
	ImGui_ImplSDL2_NewFrame(reinterpret_cast<SDL_Window*>(window->get_handle()));
	ImGui::NewFrame();
	ImGui::ShowDemoWindow();

	using namespace gfx;

	Device::get().new_frame();

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

		ImGui::Render();
		ze::ui::imgui::update();
		ze::ui::imgui::draw(list);

		list->end_render_pass();
		Device::get().submit(list);
	}

	Device::get().end_frame();
	Device::get().present(*swapchain);
#endif
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