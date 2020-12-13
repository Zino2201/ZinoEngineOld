#pragma once

#include "Engine/Engine.h"
#include <filesystem>
#include "Engine/NativeWindow.h"
#include "Gfx/Backend.h"
#include "Engine/Viewport.h"

namespace ze { class Viewport; class World; }

struct ImFont;

namespace ze::editor
{

class CMapTabWidget;
class Tab;

class ZEEDITOR_API EditorApp final : public EngineApp
{
public:
	EditorApp();
	~EditorApp();

	EditorApp(const EditorApp&) = delete;
	void operator=(const EditorApp&) = delete;

	static EditorApp& get();

	void process_event(const SDL_Event& in_event, const float in_delta_time) override;
	void post_tick(const float in_delta_time) override;
	void add_tab(OwnerPtr<Tab> in_tab);
	bool has_tab(std::string in_name);
private:
	void draw_main_tab();
	void test_renderer(const gfx::ResourceHandle& in_cmd_list);
	void on_asset_imported(const std::filesystem::path& InPath,
		const std::filesystem::path& InTarget);
private:
	NativeWindow main_window;
	gfx::UniqueSwapchain swapchain;
	gfx::UniqueCommandPool cmd_pool;
	gfx::ResourceHandle cmd_list;
	gfx::UniqueFence cmd_list_fence;
	gfx::UniqueRenderPass render_pass;
	gfx::UniqueRenderPass vp_render_pass;
	gfx::UniquePipelineLayout vp_pipeline_layout;
	gfx::UniquePipeline vp_pipeline;
	gfx::UniqueShader vs;
	gfx::UniqueShader fs;
	ImFont* font;
	std::unique_ptr<World> world;
	std::unique_ptr<CMapTabWidget> map_tab_widget;
	std::vector<std::unique_ptr<Tab>> tabs;
};

}