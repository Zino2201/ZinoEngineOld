#pragma once

#include "ImGui/ImGui.h"
#include "Engine/Viewport.h"

union SDL_Event;

namespace ze { class World; }

namespace ze::editor
{

class CEntityList;
class CEntityProperties;
class EntityProfiler;

/**
 * A map editor
 */
class MAPEDITOR_API CMapEditor
{
public:
	CMapEditor(World& in_world);
	~CMapEditor();

	void process_event(const SDL_Event& in_event, const float in_delta_time);
	void Draw(const ImGuiID& InDockspaceID);

	World& get_world() { return world; }
	Viewport& get_viewport() { return viewport; }
	bool is_viewport_fullscreen() const { return viewport_fullscreen; }

	const maths::Vector3d& get_cam_pos() const { return cam_pos; }
	const maths::Vector3f& get_cam_rot() const { return cam_rot; }
	const maths::Vector3f& get_cam_fwd() const { return cam_fwd; }
private:
	World& world;
	std::unique_ptr<CEntityList> EntityList;
	std::unique_ptr<CEntityProperties> EntityProperties;
	std::unique_ptr<EntityProfiler> entity_profiler;
	Viewport viewport;
	bool viewport_fullscreen;
	ImVec2 cached_window_size;

	/** Camera */
	bool is_moving_camera;
	maths::Vector3d cam_pos;
	maths::Vector3f cam_rot;
	maths::Vector3f cam_fwd;
	float cam_yaw;
	float cam_pitch;
};

}