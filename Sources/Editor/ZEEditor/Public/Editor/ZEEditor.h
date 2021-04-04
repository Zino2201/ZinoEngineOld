#pragma once

#include "Engine/Engine.h"
#include <filesystem>
#include "Gfx/Gfx.h"
#include "Engine/Viewport.h"
#include "ImGui/ImGuiRenderer.h"

namespace ze 
{ 
class Viewport; 
class World; 
class NativeWindow; 
}

struct ImFont;

namespace ze::editor
{

class MapEditor;

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
private:
	void on_asset_imported(const std::filesystem::path& InPath,
		const std::filesystem::path& InTarget);
private:
	ImFont* font;
	std::unique_ptr<NativeWindow> window;
	std::unique_ptr<MapEditor> map_editor;
	ui::imgui::ViewportData main_viewport_data;
};

}