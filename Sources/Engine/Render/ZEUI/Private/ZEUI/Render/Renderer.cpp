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

Renderer::Renderer(const gfx::ResourceHandle& render_pass) : 
	current_vertex_count(0), current_index_count(0), last_vertex_count(0), last_index_count(0), ubo_data(nullptr)
{
	gfx::Effect* base_effect = gfx::effect_get_by_name("ZEUIBase");
	ZE_CHECK(base_effect);
	auto permutation = base_effect->get_permutation({});
	while(!base_effect->is_available({})) 
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(50ms);
	}
	permutation = base_effect->get_permutation({});

	/** Create render resources */
	ubo = gfx::RenderBackend::get().buffer_create(
		gfx::BufferCreateInfo(
			sizeof(GlobalData),
			gfx::BufferUsageFlagBits::UniformBuffer,
			gfx::MemoryUsage::CpuToGpu));
	auto [ubo_map_result, out_ubo_data] = gfx::RenderBackend::get().buffer_map(*ubo);
	ubo_data = out_ubo_data;

	{
		auto [result, layout] = gfx::RenderBackend::get().pipeline_layout_create(
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

	auto [set_result, set_handle] = gfx::RenderBackend::get().descriptor_set_create(
		gfx::DescriptorSetCreateInfo(
			*main_pipeline_layout,
			{
				gfx::Descriptor(gfx::DescriptorType::UniformBuffer, 0, gfx::DescriptorBufferInfo(*ubo)),
			}));
	descriptor_set = set_handle;

	{
		auto [result, pipeline] = gfx::RenderBackend::get().gfx_pipeline_create(
				gfx::GfxPipelineCreateInfo(
					{
						gfx::GfxPipelineShaderStage(gfx::ShaderStageFlagBits::Vertex, 
							permutation[gfx::ShaderStageFlagBits::Vertex], "vertex"),
						gfx::GfxPipelineShaderStage(gfx::ShaderStageFlagBits::Fragment,
							permutation[gfx::ShaderStageFlagBits::Fragment], "fragment"),
					},
					gfx::PipelineVertexInputStateCreateInfo(
						Vertex::get_input_binding_desc(),
						Vertex::get_input_attribute_desc()),
					{},
					{},
					{},
					{},
					{},
					*main_pipeline_layout,
					render_pass));
		main_pipeline = pipeline;
	}
}

Renderer::~Renderer()
{
	gfx::RenderBackend::get().buffer_unmap(*ubo);
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
	if(current_vertex_count <= last_vertex_count &&
		current_index_count <= last_index_count)
		return;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	vertices.reserve(current_vertex_count);
	indices.reserve(current_index_count);

	vertex_buffer = gfx::RenderBackend::get().buffer_create(
		gfx::BufferCreateInfo(
			current_vertex_count * sizeof(Vertex),
			gfx::BufferUsageFlagBits::VertexBuffer,
			gfx::MemoryUsage::CpuToGpu));

	index_buffer = gfx::RenderBackend::get().buffer_create(
		gfx::BufferCreateInfo(
			current_index_count * sizeof(uint32_t),
			gfx::BufferUsageFlagBits::IndexBuffer,
			gfx::MemoryUsage::CpuToGpu));

	uint8_t* vertex_data = reinterpret_cast<uint8_t*>(gfx::RenderBackend::get().buffer_map(*vertex_buffer).second);
	uint8_t* index_data = reinterpret_cast<uint8_t*>(gfx::RenderBackend::get().buffer_map(*index_buffer).second);

	size_t current_vertex_offset = 0;
	size_t current_first_index = 0;

	for(const auto& context : contexts)
	{
		size_t current_vertex_idx = 0;
		for(auto& cmd : context->get_commands())
		{
			auto geometry = cmd.primitive->get_geometry(cmd);
			memcpy(vertex_data, geometry.first.data(), geometry.first.size() * sizeof(Vertex));
			memcpy(index_data, geometry.second.data(), geometry.second.size() * sizeof(uint32_t));
			vertex_data += geometry.first.size() * sizeof(Vertex);

			for(const auto& index : geometry.second)
			{
				size_t idx = current_vertex_idx + index;
				memcpy(index_data, &idx, sizeof(uint32_t));
				index_data += sizeof(uint32_t);
			}

			current_vertex_idx += geometry.first.size();
		}

		context->set_drawcall_data(current_vertex_offset, current_first_index);
		current_vertex_offset += context->get_vertex_count();
		current_first_index += context->get_index_count();
	}

	gfx::RenderBackend::get().buffer_unmap(*vertex_buffer);
	gfx::RenderBackend::get().buffer_unmap(*index_buffer);

	last_vertex_count = current_vertex_count;
	last_index_count = current_index_count;
}

void Renderer::render(Window& in_root_window, const gfx::ResourceHandle& cmd_list)
{
	/** Collect draw contexts and update vertex/index buffer if required */
	in_root_window.paint_window(*this);
	update_buffers();

	gfx::RenderBackend::get().cmd_bind_pipeline(cmd_list,
		gfx::PipelineBindPoint::Gfx,
		*main_pipeline);

	/** Update UBO */
	GlobalData gd;
	gd.scale = maths::Vector2f(2.0f / in_root_window.get_width(), 2.0f / in_root_window.get_height());
	gd.translate = maths::Vector2f(-1.0f);
	memcpy(ubo_data, &gd, sizeof(gd));

	gfx::RenderBackend::get().cmd_bind_vertex_buffers(cmd_list,
		0,
		{ *vertex_buffer },
		{ 0 });

	gfx::RenderBackend::get().cmd_bind_index_buffer(cmd_list,
		*index_buffer);

	gfx::RenderBackend::get().cmd_bind_descriptor_sets(cmd_list,
		gfx::PipelineBindPoint::Gfx,
		*main_pipeline_layout,
			0,
		{ *descriptor_set });

	/** Draw the data */
	for(const auto& context : contexts)
	{
		gfx::RenderBackend::get().cmd_draw_indexed(cmd_list,
			context->get_index_count(),
			1,
			context->get_first_index(),
			context->get_vertex_offset(),
			0);
	}
}

void Renderer::add_geometry_count(const size_t vertex_count, const size_t index_count)
{
	current_vertex_count += vertex_count;
	current_index_count += index_count;
}

}