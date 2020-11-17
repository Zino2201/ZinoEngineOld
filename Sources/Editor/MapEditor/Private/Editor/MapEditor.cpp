#include "Editor/MapEditor.h"
#include "Editor/EntityList.h"
#include "Editor/EntityProperties.h"
#include "imgui_internal.h"
#include <SDL2/SDL.h>
#include "Engine/InputSystem.h"
#include "Editor/EntityProfiler.h"

namespace ze::editor
{

CMapEditor::CMapEditor(World& in_world)
	: world(in_world), viewport(0, 0, 1280, 720), viewport_fullscreen(false), is_moving_camera(false),
	cam_fwd(maths::Vector3f::get_forward()), cam_pitch(0.f), cam_yaw(0.f)
{
	EntityList = std::make_unique<CEntityList>(in_world, *this);
	EntityProperties = std::make_unique<CEntityProperties>(*this);
	entity_profiler = std::make_unique<EntityProfiler>(*this);

	EntityList->on_entity_selected.bind(std::bind(&CEntityProperties::set_entity,
		EntityProperties.get(),
		std::placeholders::_1));
}

CMapEditor::~CMapEditor() = default;

void CMapEditor::process_event(const SDL_Event& in_event, const float in_delta_time)
{
	if(!is_moving_camera)
		return;

	ImGuiIO& io = ImGui::GetIO();

	if(in_event.type == SDL_MOUSEMOTION)
	{
		cam_yaw += input::get_mouse_delta().x;
		cam_pitch -= input::get_mouse_delta().y;
		
		if (cam_pitch > 89.0f)
			cam_pitch = 89.0f;
				
		if (cam_pitch < -89.0f)
			cam_pitch = -89.0f;

		maths::Vector3f fwd;
		fwd.x = cos(maths::radians(cam_yaw)) * cos(maths::radians(cam_pitch));
		fwd.y = sin(maths::radians(cam_yaw)) * cos(maths::radians(cam_pitch));
		fwd.z = sin(maths::radians(cam_pitch));
		cam_fwd = maths::normalize(fwd);

		SDL_WarpMouseInWindow(nullptr, io.DisplaySize.x / 2, io.DisplaySize.y / 2);
	}
}

void CMapEditor::Draw(const ImGuiID& InDockspaceID)
{
	EntityList->Draw();
	EntityProperties->Draw();
	entity_profiler->draw();

	/** Viewport */
	ImGuiWindowClass window_class;
	window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
	ImGui::SetNextWindowClass(&window_class);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	if(!ImGui::Begin("Viewport"))
	{
		ImGui::End();
		return;
	}

	if(cached_window_size.x != ImGui::GetWindowSize().x ||
		cached_window_size.y !=ImGui::GetWindowSize().y)
	{
		cached_window_size = ImGui::GetWindowSize();
		viewport.resize(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
	}

	ImGui::Image(const_cast<gfx::ResourceHandle*>(&viewport.get_color_attachment_view()), ImGui::GetContentRegionAvail());
	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
	{
		viewport_fullscreen = !viewport_fullscreen;
		if(viewport_fullscreen)
		{
			ImGuiIO& io = ImGui::GetIO();
			viewport.resize(io.DisplaySize.x, io.DisplaySize.y);
		}
	}
	else if(ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right) && !is_moving_camera)
	{
		ImGuiIO& io = ImGui::GetIO();
		is_moving_camera = true;
		SDL_WarpMouseInWindow(nullptr, io.DisplaySize.x / 2, io.DisplaySize.y / 2);
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}

	if(is_moving_camera)
	{
		if(!ImGui::IsMouseDown(ImGuiMouseButton_Right))
		{
			SDL_SetRelativeMouseMode(SDL_FALSE);
			is_moving_camera = false;
		}
		else
		{
			if(input::is_key_held(SDL_SCANCODE_W))
			{
				cam_pos += cam_fwd;
			}

			if(input::is_key_held(SDL_SCANCODE_S))
			{
				cam_pos -= cam_fwd;
			}

			if(input::is_key_held(SDL_SCANCODE_A))
			{
				cam_pos += maths::normalize(maths::cross(cam_fwd, maths::Vector3f::get_up()));
			}

			if(input::is_key_held(SDL_SCANCODE_D))
			{
				cam_pos -= maths::normalize(maths::cross(cam_fwd, maths::Vector3f::get_up()));
			}
		}
	}

	ImGui::PopStyleVar();

	ImGui::End();
}

}