#pragma once

#include "Engine/Engine.h"
#include <filesystem>
#include "Engine/NativeWindow.h"
#include "Gfx/Backend.h"

namespace ze { class Viewport; }

struct ImFont;

namespace ze::editor
{

class ZEEDITOR_API EditorApp final : public EngineApp
{
public:
	EditorApp();
	~EditorApp();

	void process_event(const SDL_Event& in_event) override;
	void post_tick(const float in_delta_time) override;
private:
	void draw_main_tab();
	void on_asset_imported(const std::filesystem::path& InPath,
		const std::filesystem::path& InTarget);
private:
	NativeWindow main_window;
	gfx::UniqueSwapchain swapchain;
	gfx::UniqueCommandPool cmd_pool;
	gfx::ResourceHandle cmd_list;
	gfx::UniqueFence cmd_list_fence;
	gfx::UniqueRenderPass render_pass;
	ImFont* font;
};

}