#include "ImGui/ImGuiRenderer.h"
#include "ImGui/ImGui.h"
#include "Gfx/Backend.h"
#include "Module/ModuleManager.h"
#include "Shader/ShaderCompiler.h"

namespace ze::ui::imgui
{

struct GlobalData
{
	maths::Vector2f scale;
	maths::Vector2f translate;
};

/** variables */
gfx::UniquePipelineLayout pipeline_layout;
gfx::UniquePipeline pipeline;
gfx::UniqueBuffer vertex_buffer;
gfx::UniqueBuffer index_buffer;
gfx::UniqueBuffer ubo;
gfx::UniqueTexture font;
gfx::UniqueTextureView font_view;
gfx::UniqueDescriptorSet descriptor_set;
gfx::UniqueShader vs;
gfx::UniqueShader fs;
gfx::UniqueSampler sampler;
void* ubo_data = nullptr;
std::unique_ptr<ImDrawData> draw_data;
std::unordered_map<gfx::ResourceHandle, gfx::ResourceHandle> descriptor_set_map;

bool initialize(const gfx::ResourceHandle& in_cmd_list, const gfx::ResourceHandle& in_renderpass)
{
	{
		/** Compile shaders */
		gfx::shaders::ShaderCompilerOutput vs_ = gfx::shaders::compile_shader(
			gfx::shaders::ShaderStage::Vertex,
			"UI/ImGuiVS.hlsl",
			"Main",
			gfx::shaders::ShaderCompilerTarget::VulkanSpirV,
			true).get();

		gfx::shaders::ShaderCompilerOutput fs_ = gfx::shaders::compile_shader(
			gfx::shaders::ShaderStage::Fragment,
			"UI/ImGuiFS.hlsl",
			"Main",
			gfx::shaders::ShaderCompilerTarget::VulkanSpirV,
			true).get();

		vs = gfx::RenderBackend::get().shader_create(gfx::ShaderCreateInfo(
			vs_.bytecode)).second;

		fs = gfx::RenderBackend::get().shader_create(gfx::ShaderCreateInfo(
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
	auto [result, handle] = gfx::RenderBackend::get().texture_create(
		gfx::TextureCreateInfo(
			gfx::TextureType::Tex2D,
			gfx::MemoryUsage::GpuOnly,
			gfx::Format::R8G8B8A8Unorm,
			width,
			height,
			1,
			1,
			1,
			gfx::SampleCountFlagBits::Count1,
			gfx::TextureUsageFlagBits::TransferDst | gfx::TextureUsageFlagBits::Sampled));
	if(result != gfx::Result::Success)
	{
		ze::logger::error("Failed to create ImGui font texture");
		return false;
	}

	font = handle;

	/** Copy to texture */
	{
		gfx::ResourceHandle staging_buf = gfx::RenderBackend::get().buffer_create(
			gfx::BufferCreateInfo(
				width * height * 4,
				gfx::BufferUsageFlagBits::TransferSrc,
				gfx::MemoryUsage::CpuOnly));
		auto [map_result, buf_data] = gfx::RenderBackend::get().buffer_map(staging_buf);
		memcpy(buf_data, data, width * height * 4);
		gfx::RenderBackend::get().buffer_unmap(staging_buf);
		
		gfx::RenderBackend::get().command_list_begin(in_cmd_list);
		gfx::RenderBackend::get().cmd_pipeline_barrier(in_cmd_list,
			gfx::PipelineStageFlagBits::TopOfPipe,
			gfx::PipelineStageFlagBits::Transfer,
			{
				gfx::TextureMemoryBarrier(*font,
					gfx::AccessFlags(),
					gfx::AccessFlagBits::TransferWrite,
					gfx::TextureLayout::Undefined,
					gfx::TextureLayout::TransferDst,
					gfx::TextureSubresourceRange(
						gfx::TextureAspectFlagBits::Color,
						0,
						1,
						0,
						1))
			});
		gfx::RenderBackend::get().cmd_copy_buffer_to_texture(in_cmd_list,
			staging_buf,
			*font,
			gfx::TextureLayout::TransferDst,
			{
				gfx::BufferTextureCopyRegion(0,
					gfx::TextureSubresourceLayers(gfx::TextureAspectFlagBits::Color,
						0,
						0,
						1),
					gfx::Offset3D(),
					gfx::Extent3D(width, height, 1))
			});
		gfx::RenderBackend::get().cmd_pipeline_barrier(in_cmd_list,
			gfx::PipelineStageFlagBits::Transfer,
			gfx::PipelineStageFlagBits::FragmentShader,
			{
				gfx::TextureMemoryBarrier(*font,
					gfx::AccessFlagBits::TransferWrite,
					gfx::AccessFlagBits::ShaderRead,
					gfx::TextureLayout::TransferDst,
					gfx::TextureLayout::ShaderReadOnly,
					gfx::TextureSubresourceRange(
						gfx::TextureAspectFlagBits::Color,
						0,
						1,
						0,
						1))
			});
		gfx::RenderBackend::get().command_list_end(in_cmd_list);
		gfx::RenderBackend::get().queue_execute(
			gfx::RenderBackend::get().get_gfx_queue(),
			{ in_cmd_list });
		// TODO: Change
		gfx::RenderBackend::get().device_wait_idle();
		gfx::RenderBackend::get().buffer_destroy(staging_buf);
	}

	font_view = gfx::RenderBackend::get().texture_view_create(
		gfx::TextureViewCreateInfo(
			*font,
			gfx::TextureViewType::Tex2D,
			gfx::Format::R8G8B8A8Unorm,
			gfx::TextureSubresourceRange(gfx::TextureAspectFlagBits::Color,
				0,
				1,
				0,
				1)));

	/** UBO, desc ... */
	ubo = gfx::RenderBackend::get().buffer_create(
		gfx::BufferCreateInfo(
			sizeof(GlobalData),
			gfx::BufferUsageFlagBits::UniformBuffer,
			gfx::MemoryUsage::CpuToGpu));
	auto [ubo_map_result, out_ubo_data] = gfx::RenderBackend::get().buffer_map(*ubo);
	ubo_data = out_ubo_data;

	auto [layout_result, pipeline_layout_handle] = gfx::RenderBackend::get().pipeline_layout_create(
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
							gfx::DescriptorType::SampledTexture,
							1,
							gfx::ShaderStageFlagBits::Fragment),
						gfx::DescriptorSetLayoutBinding(
							2,
							gfx::DescriptorType::Sampler,
							1,
							gfx::ShaderStageFlagBits::Fragment)
					})
			}));
	pipeline_layout = pipeline_layout_handle;

	auto [pipeline_result, pipeline_handle] = gfx::RenderBackend::get().gfx_pipeline_create(
		gfx::GfxPipelineCreateInfo(
			{
				gfx::GfxPipelineShaderStage(
					gfx::ShaderStageFlagBits::Vertex,
					*vs,
					"Main"),
				gfx::GfxPipelineShaderStage(
					gfx::ShaderStageFlagBits::Fragment,
					*fs,
					"Main"),
			},
			gfx::PipelineVertexInputStateCreateInfo(
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
				}),
			gfx::PipelineInputAssemblyStateCreateInfo(),
			gfx::PipelineRasterizationStateCreateInfo(),
			gfx::PipelineMultisamplingStateCreateInfo(),
			gfx::PipelineDepthStencilStateCreateInfo(),
			gfx::PipelineColorBlendStateCreationInfo(
				false, gfx::LogicOp::NoOp,
				{
					gfx::PipelineColorBlendAttachmentState(true,
						gfx::BlendFactor::SrcAlpha,
						gfx::BlendFactor::OneMinusSrcAlpha,
						gfx::BlendOp::Add,
						gfx::BlendFactor::OneMinusSrcAlpha,
						gfx::BlendFactor::Zero,
						gfx::BlendOp::Add)
				}),
			*pipeline_layout,
			in_renderpass));
	pipeline = pipeline_handle;

	auto [sampler_result, sampler_handle] = gfx::RenderBackend::get().sampler_create(
		gfx::SamplerCreateInfo());

	sampler = sampler_handle;

	auto [set_result, set_handle] = gfx::RenderBackend::get().descriptor_set_create(
		gfx::DescriptorSetCreateInfo(
			*pipeline_layout,
			{
				gfx::Descriptor(gfx::DescriptorType::UniformBuffer, 0, gfx::DescriptorBufferInfo(*ubo)),
				gfx::Descriptor(gfx::DescriptorType::SampledTexture, 1, gfx::DescriptorTextureInfo(*font_view, gfx::TextureLayout::ShaderReadOnly)),
				gfx::Descriptor(gfx::DescriptorType::Sampler, 2, gfx::DescriptorTextureInfo(*sampler)),
			}));
	descriptor_set = set_handle;

	draw_data = std::make_unique<ImDrawData>();

	return true;
}

void free_all_sets()
{
	for(const auto& [blc, set] : descriptor_set_map)
		gfx::RenderBackend::get().descriptor_set_destroy(set);

	descriptor_set_map.clear();
}

void destroy()	
{
	gfx::RenderBackend::get().buffer_unmap(*ubo);

	free_all_sets();
	descriptor_set.reset();
	pipeline_layout.reset();
	pipeline.reset();
	vertex_buffer.reset();
	index_buffer.reset();
	ubo.reset();
	font.reset();
	font_view.reset();
	sampler.reset();
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

	/** Copy draw data*/
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
		vertex_buffer = gfx::RenderBackend::get().buffer_create(
			gfx::BufferCreateInfo(
				vertex_size,
				gfx::BufferUsageFlagBits::VertexBuffer,
				gfx::MemoryUsage::CpuToGpu));
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
		index_buffer = gfx::RenderBackend::get().buffer_create(
			gfx::BufferCreateInfo(
				index_size,
				gfx::BufferUsageFlagBits::IndexBuffer,
				gfx::MemoryUsage::CpuToGpu));
		if (!index_buffer)
		{
			ze::logger::error("Failed to create ImGui index buffer");
			return;
		}

		index_count = draw_data->TotalIdxCount;
		last_index_size = index_size;
	}

	/** Write data to both buffers */
	auto vertex_map = gfx::RenderBackend::get().buffer_map(*vertex_buffer);
	auto index_map = gfx::RenderBackend::get().buffer_map(*index_buffer);
	
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

	gfx::RenderBackend::get().buffer_unmap(*vertex_buffer);
	gfx::RenderBackend::get().buffer_unmap(*index_buffer);
}

gfx::ResourceHandle get_or_create_descriptor_set(const gfx::ResourceHandle& in_texture_view)
{
	auto set = descriptor_set_map.find(in_texture_view);
	if(set != descriptor_set_map.end())
		return set->second;

	auto [set_result, set_handle] = gfx::RenderBackend::get().descriptor_set_create(
		gfx::DescriptorSetCreateInfo(
			*pipeline_layout,
			{	
				gfx::Descriptor(gfx::DescriptorType::UniformBuffer, 0, gfx::DescriptorBufferInfo(*ubo)),
				gfx::Descriptor(gfx::DescriptorType::SampledTexture, 1, gfx::DescriptorTextureInfo(in_texture_view, 
					gfx::TextureLayout::ShaderReadOnly)),
				gfx::Descriptor(gfx::DescriptorType::Sampler, 2, gfx::DescriptorTextureInfo(*sampler)),
			}));
	descriptor_set_map.insert({ in_texture_view, set_handle });

	return set_handle;
}

void draw(const gfx::ResourceHandle& in_cmd_list)
{
	ImGuiIO& io = ImGui::GetIO();

	free_all_sets();

	/** Update UBO */
	GlobalData gd;
	gd.scale = maths::Vector2f(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
	gd.translate = maths::Vector2f(-1.0f);
	memcpy(ubo_data, &gd, sizeof(gd));

	gfx::RenderBackend::get().cmd_bind_pipeline(in_cmd_list,
		gfx::PipelineBindPoint::Gfx,
		*pipeline);

	uint32_t vertex_offset = 0;
	uint32_t index_offset = 0;
	uint64_t vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
	uint64_t index_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);

	if (draw_data->CmdListsCount > 0 &&
		last_vertex_size >= vertex_size &&
		last_index_size >= index_size)
	{
		gfx::RenderBackend::get().cmd_bind_vertex_buffers(in_cmd_list, 0, { *vertex_buffer }, { 0 });
		gfx::RenderBackend::get().cmd_bind_index_buffer(in_cmd_list, *index_buffer, 0, gfx::IndexType::Uint16);
		for (int32_t i = 0; i < draw_data->CmdListsCount; i++)
		{
			ImDrawList* cmd_list = draw_data->CmdLists[i];
			for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
			{
				ImDrawCmd* cmd = &cmd_list->CmdBuffer[j];
				
				if(cmd->TextureId)
				{
					gfx::RenderBackend::get().cmd_bind_descriptor_sets(in_cmd_list,
						gfx::PipelineBindPoint::Gfx,
						*pipeline_layout,
						0,
						{ get_or_create_descriptor_set(*reinterpret_cast<gfx::ResourceHandle*>(cmd->TextureId)) });
				}
				else
				{
					gfx::RenderBackend::get().cmd_bind_descriptor_sets(in_cmd_list,
						gfx::PipelineBindPoint::Gfx,
						*pipeline_layout,
						0,
						{ *descriptor_set });
				}

				gfx::RenderBackend::get().cmd_set_scissor(in_cmd_list,
					0,
					{
						maths::Rect2D(
							maths::Vector2f(std::max<int32_t>(cmd->ClipRect.x, 0),
								std::max<int32_t>(cmd->ClipRect.y, 0)),
							maths::Vector2f(
								cmd->ClipRect.z - cmd->ClipRect.x,
								cmd->ClipRect.w - cmd->ClipRect.y))
					});

				gfx::RenderBackend::get().cmd_draw_indexed(in_cmd_list,
					cmd->ElemCount,
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