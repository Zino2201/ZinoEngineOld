#include "ImGui/ImGuiRenderer.h"
#include "ImGui/ImGui.h"
#include "Gfx/Gfx.h"
#include "Module/ModuleManager.h"
#include "Shader/ShaderCompiler.h"
#include "ZEFS/Utils.h"

namespace ze::ui::imgui
{

struct GlobalData
{
	maths::Vector2f scale;
	maths::Vector2f translate;
};

/** variables */
gfx::UniquePipelineLayout pipeline_layout;
gfx::UniqueBuffer vertex_buffer;
gfx::UniqueBuffer index_buffer;
gfx::UniqueBuffer ubo;
gfx::UniqueTexture font;
gfx::UniqueTextureView font_view;
gfx::UniqueShader vs;
gfx::UniqueShader fs;
void* ubo_data = nullptr;
std::unique_ptr<ImDrawData> draw_data;
gfx::GfxPipelineRenderPassState render_pass_state;
gfx::GfxPipelineInstanceState instance_state;

bool initialize(const gfx::ResourceHandle& in_cmd_list, const gfx::ResourceHandle& in_renderpass)
{
	{
		/** Compile shaders */
		gfx::shaders::ShaderCompilerOutput vs_ = gfx::shaders::compile_shader(
			gfx::shaders::ShaderStage::Vertex,
			"ImGuiVS",
			filesystem::read_file_to_string("Shaders/UI/ImGuiVS.hlsl"),
			"Main",
			gfx::shaders::ShaderCompilerTarget::VulkanSpirV,
			true);

		gfx::shaders::ShaderCompilerOutput fs_ = gfx::shaders::compile_shader(
			gfx::shaders::ShaderStage::Fragment,
			"ImGuiFS",
			filesystem::read_file_to_string("Shaders/UI/ImGuiFS.hlsl"),
			"Main",
			gfx::shaders::ShaderCompilerTarget::VulkanSpirV,
			true);

		vs = gfx::Device::get().create_shader(gfx::ShaderCreateInfo(
			vs_.bytecode)).second;

		fs = gfx::Device::get().create_shader(gfx::ShaderCreateInfo(
			fs_.bytecode)).second;
	}

	ImGuiIO& io = ImGui::GetIO();

	/**
	 * Create font texture
	 */
	uint8_t* data = nullptr;
	uint32_t width = 0;
	uint32_t height = 0;
	io.Fonts->GetTexDataAsRGBA32(&data, reinterpret_cast<int*>(&width), 
		reinterpret_cast<int*>(&height));
	auto [result, handle] = gfx::Device::get().create_texture(
		gfx::TextureInfo::make_2d_texture(
			width, 
			height, 
			gfx::Format::R8G8B8A8Unorm,
			1,
			gfx::TextureUsageFlagBits::Sampled),
		gfx::TextureInfo::InitialData(std::span<uint8_t>(data, width * height * 4),
			gfx::PipelineStageFlagBits::FragmentShader,
			gfx::TextureLayout::ShaderReadOnly,
			gfx::AccessFlagBits::ShaderRead));
	if(result != gfx::Result::Success)
	{
		ze::logger::error("Failed to create ImGui font texture");
		return false;
	}

	font = handle;

	font_view = gfx::Device::get().create_texture_view(
		gfx::TextureViewInfo::make_2d_view(
			*font,
			gfx::Format::R8G8B8A8Unorm,
			gfx::TextureSubresourceRange(gfx::TextureAspectFlagBits::Color,
				0,
				1,
				0,
				1))).second;

	/** UBO, desc ... */
	ubo = gfx::Device::get().create_buffer(gfx::BufferInfo::make_ubo(sizeof(GlobalData))).second;
	auto [ubo_map_result, out_ubo_data] = gfx::Device::get().map_buffer(*ubo);
	ubo_data = out_ubo_data;

	auto [layout_result, pipeline_layout_handle] = gfx::Device::get().create_pipeline_layout(
		gfx::PipelineLayoutCreateInfo(
			{
				gfx::DescriptorSetLayoutCreateInfo(
					{
						gfx::DescriptorSetLayoutBinding(
							0,
							gfx::DescriptorType::UniformBuffer,
							1,
							gfx::ShaderStageFlagBits::Vertex),
						gfx::DescriptorSetLayoutBinding(
							1,
							gfx::DescriptorType::Sampler,
							1,
							gfx::ShaderStageFlagBits::Fragment),
						gfx::DescriptorSetLayoutBinding(
							2,
							gfx::DescriptorType::SampledTexture,
							1,
							gfx::ShaderStageFlagBits::Fragment)
					})
			}));
	pipeline_layout = pipeline_layout_handle;

	render_pass_state.color_blend = gfx::PipelineColorBlendStateCreationInfo(
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

	instance_state.shaders = {
		gfx::GfxPipelineShaderStageInfo(
			gfx::ShaderStageFlagBits::Vertex,
			*vs,
			"Main"),
		gfx::GfxPipelineShaderStageInfo(
			gfx::ShaderStageFlagBits::Fragment,
			*fs,
			"Main"),
	};
	instance_state.vertex_input = gfx::PipelineVertexInputStateCreateInfo(
		{
			gfx::VertexInputBindingDescription(0, 
				sizeof(ImDrawVert),
				gfx::VertexInputRate::Vertex)
		},
		{
			gfx::VertexInputAttributeDescription(0, 0, 
				gfx::Format::R32G32Sfloat, offsetof(ImDrawVert, pos)),
			gfx::VertexInputAttributeDescription(1, 0, 
				gfx::Format::R32G32Sfloat, offsetof(ImDrawVert, uv)),
			gfx::VertexInputAttributeDescription(2, 0,
				gfx::Format::R8G8B8A8Unorm, offsetof(ImDrawVert, col)),
		});

	draw_data = std::make_unique<ImDrawData>();

	return true;
}

void destroy()	
{
	gfx::Device::get().unmap_buffer(*ubo);

	pipeline_layout.reset();
	vertex_buffer.reset();
	index_buffer.reset();
	ubo.reset();
	font.reset();
	font_view.reset();
	vs.reset();
	fs.reset();
}

uint64_t last_vertex_size = 0;
uint64_t last_index_size = 0;
uint64_t vertex_count = 0;
uint64_t index_count = 0;

void update()
{
	ZE_CHECK(draw_data);

	/** Copy draw data */
	{
		ImDrawData* original_draw_data = ImGui::GetDrawData();
		if (!original_draw_data)
			return;

		bool recreate_cmd_list = original_draw_data->CmdListsCount != draw_data->CmdListsCount;
		ImDrawList** draw_list = draw_data->CmdLists;

		for (size_t i = 0; i < draw_data->CmdListsCount; ++i)
		{
			IM_DELETE(draw_data->CmdLists[i]);
		}

		if (recreate_cmd_list)
			delete draw_data->CmdLists;

		memcpy(draw_data.get(), original_draw_data, sizeof(ImDrawData));
		draw_data->CmdLists = draw_list;

		if (recreate_cmd_list)
			draw_data->CmdLists = new ImDrawList*[draw_data->CmdListsCount];

		for (size_t i = 0; i < draw_data->CmdListsCount; ++i)
		{
			draw_data->CmdLists[i] = original_draw_data->CmdLists[i]->CloneOutput();
		}
	}

	uint64_t vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
	uint64_t index_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);

	if (vertex_size == 0 || index_size == 0)
		return;

	/** Recreate buffers if modified and needed */
	if (!vertex_buffer || last_vertex_size < vertex_size)
	{
		vertex_buffer = gfx::Device::get().create_buffer(
			gfx::BufferInfo::make_vertex_buffer_cpu_visible(
				vertex_size)).second;
		if (!vertex_buffer)
		{
			ze::logger::error("Failed to create ImGui vertex buffer");
			return;
		}

		vertex_count = draw_data->TotalVtxCount;
		last_vertex_size = vertex_size;
	}

	if (!index_buffer || last_index_size < index_size)
	{
		index_buffer = gfx::Device::get().create_buffer(
			gfx::BufferInfo::make_index_buffer_cpu_visible(
				index_size)).second;
		if (!index_buffer)
		{
			ze::logger::error("Failed to create ImGui index buffer");
			return;
		}

		index_count = draw_data->TotalIdxCount;
		last_index_size = index_size;
	}

	/** Write data to both buffers */
	auto vertex_map = gfx::Device::get().map_buffer(*vertex_buffer);
	auto index_map = gfx::Device::get().map_buffer(*index_buffer);
	
	ImDrawVert* vertex_data = reinterpret_cast<ImDrawVert*>(vertex_map.second);
	ImDrawIdx* index_data = reinterpret_cast<ImDrawIdx*>(index_map.second);

	for (int i = 0; i < draw_data->CmdListsCount; i++)
	{
		ImDrawList* cmd_list = draw_data->CmdLists[i];
		memcpy(vertex_data, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		memcpy(index_data, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		vertex_data += cmd_list->VtxBuffer.Size;
		index_data += cmd_list->IdxBuffer.Size;
	}

	gfx::Device::get().unmap_buffer(*vertex_buffer);
	gfx::Device::get().unmap_buffer(*index_buffer);
}

void draw(gfx::CommandList* in_list)
{
	ImGuiIO& io = ImGui::GetIO();

	/** Update UBO */
	GlobalData gd;
	gd.scale = maths::Vector2f(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
	gd.translate = maths::Vector2f(-1.0f);
	memcpy(ubo_data, &gd, sizeof(gd));

	in_list->bind_pipeline_layout(*pipeline_layout);
	in_list->set_pipeline_render_pass_state(render_pass_state);
	in_list->set_pipeline_instance_state(instance_state);

	uint32_t vertex_offset = 0;
	uint32_t index_offset = 0;
	uint64_t vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
	uint64_t index_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);

	in_list->bind_ubo(0, 0, *ubo);
	in_list->bind_sampler(0, 1, gfx::LinearSampler::get());

	if (draw_data->CmdListsCount > 0 &&
		last_vertex_size >= vertex_size &&
		last_index_size >= index_size)
	{
		in_list->bind_vertex_buffer(*vertex_buffer, 0);
		in_list->bind_index_buffer(*index_buffer, 0, gfx::IndexType::Uint16);

		for (int32_t i = 0; i < draw_data->CmdListsCount; i++)
		{
			ImDrawList* cmd_list = draw_data->CmdLists[i];
			for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
			{
				ImDrawCmd* cmd = &cmd_list->CmdBuffer[j];

				if(cmd->TextureId)
				{
					in_list->bind_texture(0, 2, *reinterpret_cast<gfx::DeviceResourceHandle*>(cmd->TextureId));
				}
				else
				{
					in_list->bind_texture(0, 2, *font_view);
				}

				in_list->set_scissor(maths::Rect2D(
					maths::Vector2f(std::max<int32_t>(cmd->ClipRect.x, 0),
						std::max<int32_t>(cmd->ClipRect.y, 0)),
					maths::Vector2f(
						cmd->ClipRect.z - cmd->ClipRect.x,
						cmd->ClipRect.w - cmd->ClipRect.y)));

				in_list->draw_indexed(cmd->ElemCount,
					1,
					cmd->IdxOffset + index_offset,
					cmd->VtxOffset + vertex_offset,
					0);
			}

			vertex_offset += cmd_list->VtxBuffer.Size;
			index_offset += cmd_list->IdxBuffer.Size;
		}
	}
}

}