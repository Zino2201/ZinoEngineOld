#pragma once

#include "Engine/Engine.h"
#include <filesystem>
#include "Gfx/Gfx.h"
#include "Engine/Viewport.h"
#include "ImGui/ImGuiRenderer.h"
#include "Maths/Vector.h"
#include "Maths/Matrix.h"

namespace ze 
{ 
class Viewport; 
class World; 
class NativeWindow; 
}

struct ModelVertexCon
{
	ze::maths::Vector3f position;
	ze::maths::Vector3f normal;
	ze::maths::Vector2f uv;

	bool operator==(const ModelVertexCon& other) const
	{
		return position == other.position;
	}
};

namespace std
{

	template<> struct hash<ModelVertexCon>
	{
		ZE_FORCEINLINE uint64_t operator()(const ModelVertexCon& in_vertex) const
		{
			uint64_t hash = 0;

			ze::hash_combine(hash, in_vertex.position);
			ze::hash_combine(hash, in_vertex.uv);
			ze::hash_combine(hash, in_vertex.normal);

			return hash;
		}
	};
}

struct ImFont;

namespace ze::editor
{

class Window;

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

	void add_window(OwnerPtr<Window> in_window);
	bool has_window(const std::string& in_title);
private:
	void on_asset_imported(const std::filesystem::path& InPath,
		const std::filesystem::path& InTarget);
private:
	struct TEEST
	{
		maths::Matrix4f wvp;
		maths::Matrix4f world;
		maths::Vector3f cam_pos;
	};

	ImFont* font;
	std::unique_ptr<NativeWindow> window;
	std::vector<std::unique_ptr<Window>> main_windows;
	std::vector<std::unique_ptr<Window>> main_windows_queue;
	ui::imgui::ViewportData main_viewport_data;
	gfx::UniqueBuffer landscape_vbuffer;
	gfx::UniqueBuffer landscape_ibuffer;
	gfx::UniqueShader shader_vert;
	gfx::UniqueShader shader_frag;
	gfx::UniqueTexture depth_texture;
	gfx::UniqueTextureView depth_view;
	gfx::UniformBuffer<TEEST> ubo_wvp;
	gfx::UniquePipelineLayout landscape_playout;
	std::vector<ModelVertexCon> landscape_vertices;
	std::vector<uint32_t> landscape_indices;
	maths::Vector3f cam_pos;
	maths::Vector3f cam_fwd;
	float cam_pitch = 0.f, cam_yaw = 0.f;
};

}