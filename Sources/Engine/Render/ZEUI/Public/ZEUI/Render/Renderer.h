#pragma once

#include "EngineCore.h"
#include "DrawCommand.h"
#include "Gfx/Backend.h"
#include "Gfx/Effect/Effect.h"
#include <vector>

namespace ze::ui
{

class Renderer;
class Window;

/**
 * A batch of draw commands
 * Batched using the Effect to use
 */
struct DrawCommandBatch
{
	DrawCommandBatch(const gfx::EffectPermPtr& in_effect,
		const std::vector<gfx::ResourceHandle>& in_descriptor_sets) : effect(in_effect), descriptor_sets(in_descriptor_sets),
		vertex_offset(0), first_index(0), vertex_count(0), index_count(0) {}

	/**
	 * Effect to use, if effect is nullptr then it will use the default ZEUIBase effect
	 */
	gfx::EffectPermPtr effect;
	
	/**
	 * Descriptor sets to bind when drawing this batch
	 */
	std::vector<gfx::ResourceHandle> descriptor_sets;

	/** Commands contained in this batch */
	std::vector<const DrawCommand*> commands;

	size_t vertex_offset;
	size_t first_index;
	size_t vertex_count;
	size_t index_count;

	bool can_batch(const DrawCommand& command) const
	{
		return command.primitive->get_effect() == effect &&
			command.primitive->get_descriptor_sets() == descriptor_sets;
	}
};

/**
 * A context used to hold drawcommands and a scissor rect
 * Usually one per window
 */
class DrawContext
{
public:
	DrawContext(Renderer& in_renderer) : renderer(in_renderer) {}

	template<typename PrimitiveType, typename... Args>
	void add(Args&&... args)
	{
		commands.emplace_back(new PrimitiveType, std::forward<Args>(args)...);
		renderer.add_geometry_count(commands.back().primitive->get_geometry(commands.back()).first.size(), 
			commands.back().primitive->get_geometry(commands.back()).second.size());
	}	

	template<typename... Args>
	void add(OwnerPtr<DrawCommandPrimitive> primitive, Args&&... args)
	{
		commands.emplace_back(primitive, std::forward<Args>(args)...);
		renderer.add_geometry_count(commands.back().primitive->get_geometry(commands.back()).first.size(), 
			commands.back().primitive->get_geometry(commands.back()).second.size());
	}	

	/**
	 * Sort the commands array so that each commands that use the same effect are close
	 */
	void sort()
	{
		std::sort(commands.begin(), commands.end(),
			[](const DrawCommand& left, const DrawCommand& right)
			{
				return left.primitive->get_effect() == right.primitive->get_effect();
			});
	}

	/**
	 * Batch commands
	 * The commands array MUST be sorted !
	 */
	void batch()
	{
		if(commands.empty())
			return;

		DrawCommand& first = commands[0];
		DrawCommandBatch* batch = &batches.emplace_back(first.primitive->get_effect(),
			first.primitive->get_descriptor_sets());
		batch->commands.emplace_back(&first);
		batch->vertex_count += first.primitive->get_geometry(first).first.size();
		batch->index_count += first.primitive->get_geometry(first).second.size();

		for(const auto& command : commands)
		{
			/** Skip the first */
			if(command == first)
				continue;

			/** If we can't batch this, create a new batch */
			if(!batch->can_batch(command))
			{
				batch = &batches.emplace_back(command.primitive->get_effect(),
					command.primitive->get_descriptor_sets());
			}

			batch->commands.emplace_back(&command);
			batch->vertex_count += command.primitive->get_geometry(command).first.size();
			batch->index_count += command.primitive->get_geometry(command).second.size();
		}
	}

	ZE_FORCEINLINE const auto& get_commands() const { return commands; }
	ZE_FORCEINLINE auto& get_batches() { return batches; }
private:
	Renderer& renderer;
	std::vector<DrawCommand> commands;
	std::vector<DrawCommandBatch> batches;
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

	/** Actual batches that will be renderer, this is a copy of the last used batches to generate the buffers */
	std::vector<DrawCommandBatch> batches;
};

}