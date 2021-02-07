#pragma once

#include "EngineCore.h"
#include "DrawCommand.h"
#include "Gfx/Backend.h"
#include <vector>

namespace ze::ui
{

class Renderer;
class Window;

/**
 * A context used to hold drawcommands and a scissor rect
 * Usually one per window
 */
class DrawContext
{
public:
	DrawContext(Renderer& in_renderer) : renderer(in_renderer), vertex_offset(0), first_index(0), vertex_count(0), index_count(0) {}

	template<typename PrimitiveType, typename... Args>
	void add(Args&&... args)
	{
		commands.emplace_back(new PrimitiveType, std::forward<Args>(args)...);
		vertex_count += commands.back().primitive->get_geometry(commands.back()).first.size();
		index_count += commands.back().primitive->get_geometry(commands.back()).second.size();
		renderer.add_geometry_count(vertex_count, index_count);
	}	

	void set_drawcall_data(const size_t in_vertex_offset, const size_t in_first_index)
	{
		vertex_offset = in_vertex_offset;
		first_index = in_first_index;
	}

	ZE_FORCEINLINE const auto& get_commands() const { return commands; }
	ZE_FORCEINLINE const size_t get_vertex_offset() const { return vertex_offset; }
	ZE_FORCEINLINE const size_t get_first_index() const { return first_index; }
	ZE_FORCEINLINE const size_t get_vertex_count() const { return vertex_count; }
	ZE_FORCEINLINE const size_t get_index_count() const { return index_count; }
private:
	Renderer& renderer;
	std::vector<DrawCommand> commands;
	size_t vertex_offset;
	size_t first_index;
	size_t vertex_count;
	size_t index_count;
};

/**
 * Manage the draw contexts and global rendering
 */
class Renderer
{
public:
	Renderer(const gfx::ResourceHandle& render_pass);
	~Renderer();

	DrawContext& create_context();

	/**
	 * Begin new frame
	 * Destroy contexts
	 */
	void new_frame();

	/**
	 * Render a window
	 * Will update buffers if required
	 */
	void render(Window& in_root_window, const gfx::ResourceHandle& cmd_list);

	void add_geometry_count(const size_t vertex_count, const size_t index_count);
private:
	void update_buffers();
private:
	std::vector<std::unique_ptr<DrawContext>> contexts;
	gfx::UniqueBuffer vertex_buffer;
	gfx::UniqueBuffer index_buffer;
	size_t current_vertex_count;
	size_t current_index_count;
	size_t last_vertex_count;
	size_t last_index_count;
	gfx::UniquePipeline main_pipeline;
	gfx::UniquePipelineLayout main_pipeline_layout;
	gfx::UniqueBuffer ubo;
	gfx::UniqueDescriptorSet descriptor_set;
	void* ubo_data;
};

}