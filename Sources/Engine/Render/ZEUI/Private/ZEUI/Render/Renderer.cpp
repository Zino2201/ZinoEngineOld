#include "ZEUI/Render/Renderer.h"
#include "ZEUI/Window.h"
#include "Gfx/Effect/Effect.h"
#include "Gfx/Effect/EffectDatabase.h"

namespace ze::ui
{

struct GlobalData
{
	maths::Vector2f scale;
	maths::Vector2f translate;
};

Renderer::Renderer() : 
	current_vertex_count(0), current_index_count(0), last_vertex_count(0), last_index_count(0), ubo_data(nullptr)
{
	gfx::Effect* base_effect = gfx::effect_get_by_name("ZEUIBase");
	ZE_CHECK(base_effect);
	permutation = base_effect->get_permutation({});
	while(!base_effect->is_available({})) 
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(50ms);
	}

	permutation = base_effect->get_permutation({});

	/** Create render resources */
	ubo = gfx::Device::get().create_buffer(gfx::BufferInfo::make_ubo(sizeof(GlobalData))).second;
	auto [ubo_map_result, out_ubo_data] = gfx::Device::get().map_buffer(*ubo);
	ubo_data = out_ubo_data;

	{
		auto [result, layout] = gfx::Device::get().create_pipeline_layout(
			gfx::PipelineLayoutCreateInfo(
				{
					gfx::DescriptorSetLayoutCreateInfo(
					{
						gfx::DescriptorSetLayoutBinding(
							0,
							gfx::DescriptorType::UniformBuffer,
							1,
							gfx::ShaderStageFlagBits::Vertex),
					})
				}));
		main_pipeline_layout = layout;
	}
}

Renderer::~Renderer()
{
	gfx::Device::get().unmap_buffer(*ubo);
}

void Renderer::new_frame()
{
	contexts.clear();

	current_vertex_count = 0;
	current_index_count = 0;
}

DrawContext& Renderer::create_context()
{
	auto& context = contexts.emplace_back(std::make_unique<DrawContext>(*this));
	return *context;
}

void Renderer::update_buffers()
{ 
	if(current_vertex_count > last_vertex_count && 
		current_index_count > last_index_count)
	{
		vertex_buffer = gfx::Device::get().create_buffer(gfx::BufferInfo::make_vertex_buffer_cpu_visible(current_vertex_count * sizeof(Vertex))).second;
		index_buffer = gfx::Device::get().create_buffer(gfx::BufferInfo::make_index_buffer_cpu_visible(current_index_count * sizeof(uint32_t))).second;
	}

	if(!vertex_buffer || !index_buffer)
		return;

	/** Build the vertex/index buffers */
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	vertices.reserve(current_vertex_count);
	indices.reserve(current_index_count);

	uint8_t* vertex_data = reinterpret_cast<uint8_t*>(gfx::Device::get().map_buffer(*vertex_buffer).second);
	uint8_t* index_data = reinterpret_cast<uint8_t*>(gfx::Device::get().map_buffer(*index_buffer).second);

	size_t current_vertex_offset = 0;
	size_t current_first_index = 0;

	for(auto& context : contexts)
	{
		/** Sort and batch the context commands first */
		context->sort();
		context->batch();

		size_t current_vertex_idx = 0;

		for(auto& batch : context->get_batches())
		{
			for(const auto& cmd : batch.commands)
			{
				memcpy(vertex_data, cmd->primitive->get_vertices().data(), cmd->primitive->get_vertices().size() * sizeof(Vertex));
				memcpy(index_data, cmd->primitive->get_indices().data(), cmd->primitive->get_indices().size() * sizeof(uint32_t));
				vertex_data += cmd->primitive->get_vertices().size() * sizeof(Vertex);

				for(const auto& index : cmd->primitive->get_indices())
				{
					size_t idx = current_vertex_idx + index;
					memcpy(index_data, &idx, sizeof(uint32_t));
					index_data += sizeof(uint32_t);
				}

				current_vertex_idx += cmd->primitive->get_vertices().size();
			}

			batch.vertex_offset = current_vertex_offset;
			batch.first_index = current_first_index;
			current_vertex_offset += batch.vertex_count;
			current_first_index += batch.index_count;
		}
	}

	gfx::Device::get().unmap_buffer(*vertex_buffer);
	gfx::Device::get().unmap_buffer(*index_buffer);

	last_vertex_count = current_vertex_count;
	last_index_count = current_index_count;
}

void Renderer::render(Window& in_root_window, gfx::CommandList* list)
{
	/** Collect draw contexts and update vertex/index buffer if required */
	in_root_window.paint_window(*this);
	update_buffers();

	if(!vertex_buffer || !index_buffer)
		return;

	gfx::GfxPipelineRenderPassState rp_state;
	rp_state.color_blend = gfx::PipelineColorBlendStateCreationInfo(
		false, gfx::LogicOp::NoOp,
		{
			gfx::PipelineColorBlendAttachmentState(true,
				gfx::BlendFactor::SrcAlpha,
				gfx::BlendFactor::OneMinusSrcAlpha,
				gfx::BlendOp::Add,
				gfx::BlendFactor::OneMinusSrcAlpha,
				gfx::BlendFactor::Zero,
				gfx::BlendOp::Add)
		});
	list->set_pipeline_render_pass_state(rp_state);

	/** Update UBO */
	GlobalData gd;
	gd.scale = maths::Vector2f(2.0f / in_root_window.get_width(), 2.0f / in_root_window.get_height());
	gd.translate = maths::Vector2f(-1.0f);
	memcpy(ubo_data, &gd, sizeof(gd));

	list->bind_vertex_buffer(*vertex_buffer, 0);
	list->bind_index_buffer(*index_buffer);

	/** Draw the data */
	for(const auto& context : contexts)
	{
		for(const auto& batch : context->get_batches())
		{
			/** Bind desc set and shaders */
			list->bind_pipeline_layout(*batch.effect->pipeline_layout);
			list->bind_ubo(0, 0, *ubo);
			for(const auto& binding : batch.bindings)
			{
				switch(binding.type)
				{
				case gfx::DescriptorType::UniformBuffer:
					list->bind_ubo(binding.set, binding.binding, binding.resource);
					break;
				case gfx::DescriptorType::StorageBuffer:
					list->bind_ssbo(binding.set, binding.binding, binding.resource);
					break;
				case gfx::DescriptorType::Sampler:
					list->bind_sampler(binding.set, binding.binding, binding.resource);
					break;
				case gfx::DescriptorType::SampledTexture:
					list->bind_texture(binding.set, binding.binding, binding.resource);
					break;
				}
			}

			gfx::GfxPipelineInstanceState state;
			state.shaders = 
			{ 
				gfx::GfxPipelineShaderStageInfo(gfx::ShaderStageFlagBits::Vertex,
					batch.effect->shader_map[gfx::ShaderStageFlagBits::Vertex],
					"vertex"),
				gfx::GfxPipelineShaderStageInfo(gfx::ShaderStageFlagBits::Fragment,
					batch.effect->shader_map[gfx::ShaderStageFlagBits::Fragment],
					"fragment")
			};
			state.vertex_input.input_binding_descriptions = Vertex::get_input_binding_desc();
			state.vertex_input.input_attribute_descriptions = Vertex::get_input_attribute_desc();
			list->set_pipeline_instance_state(state);
			list->draw_indexed(batch.index_count,
				1,
				batch.first_index,
				batch.vertex_offset,
				0);
		}
	}
}

void Renderer::add_geometry_count(const size_t vertex_count, const size_t index_count)
{
	current_vertex_count += vertex_count;
	current_index_count += index_count;
}

}