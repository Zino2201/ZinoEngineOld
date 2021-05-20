#include "Pipeline.h"
#include "VulkanBackend.h"
#include "VulkanUtil.h"
#include "Command.h"
#include "Device.h"
#include "Sync.h"
#include "Texture.h"
#include "PipelineLayout.h"
#include "RenderPass.h"
#include "Shader.h"
#include <robin_hood.h>

namespace ze::gfx::vulkan
{

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
robin_hood::unordered_set<ResourceHandle> pipelines;
#endif

vk::Result last_pipeline_result;

std::pair<Result, ResourceHandle> VulkanBackend::gfx_pipeline_create(const GfxPipelineCreateInfo& in_create_info)
{
	ResourceHandle handle = create_resource<Pipeline>(*device, in_create_info);
#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	pipelines.insert(handle);
#endif

	return { convert_vk_result(last_pipeline_result), handle };
}

void VulkanBackend::pipeline_destroy(const ResourceHandle& in_handle)
{
	delete_resource<Pipeline>(in_handle);

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	pipelines.erase(in_handle);
#endif
}

Pipeline::Pipeline(Device& in_device, const GfxPipelineCreateInfo& in_create_info)
	: device(in_device)
{
	PipelineLayout* layout = PipelineLayout::get(in_create_info.pipeline_layout);
	ZE_CHECKF(layout, "Invalid pipeline layout given to gfx_pipeline_create");

	RenderPass* render_pass = RenderPass::get(in_create_info.render_pass);
	ZE_CHECKF(render_pass, "Invalid render pass given to gfx_pipeline_create");

	std::vector<vk::PipelineShaderStageCreateInfo> shader_stages;
	shader_stages.reserve(in_create_info.shader_stages.size());
	for(const auto& stage : in_create_info.shader_stages)
	{
		Shader* shader = Shader::get(stage.shader);
		ZE_CHECKF(shader, "Invalid shader handle given to GfxPipeline");

		shader_stages.emplace_back(
			vk::PipelineShaderStageCreateFlags(),
			convert_shader_stage_bit(stage.shader_stage),
			shader->get_shader(),
			stage.entry_point);
	}

	std::vector<vk::VertexInputBindingDescription> input_binding_descriptions;
	input_binding_descriptions.reserve(in_create_info.vertex_input_state.input_binding_descriptions.size());
	for(const auto& desc : in_create_info.vertex_input_state.input_binding_descriptions)
	{
		input_binding_descriptions.emplace_back(
			desc.binding,
			desc.stride,
			convert_vertex_input_rate(desc.input_rate));
	}
	
	std::vector<vk::VertexInputAttributeDescription> input_attribute_descriptions;
	input_attribute_descriptions.reserve(in_create_info.vertex_input_state.input_attribute_descriptions.size());
	for(const auto& desc : in_create_info.vertex_input_state.input_attribute_descriptions)
	{
		input_attribute_descriptions.emplace_back(
			desc.location,
			desc.binding,
			convert_format(desc.format),
			desc.offset);
	}

	vk::PipelineVertexInputStateCreateInfo vertex_input_state(
		vk::PipelineVertexInputStateCreateFlags(),
		input_binding_descriptions,
		input_attribute_descriptions);

	vk::PipelineInputAssemblyStateCreateInfo input_assembly_state(
		vk::PipelineInputAssemblyStateCreateFlags(),
		convert_primitive_topology(in_create_info.input_assembly_state.primitive_topology),
		VK_FALSE);

	std::array<vk::Viewport, 1> default_viewport = 
	{
		vk::Viewport()
	};

	std::array<vk::Rect2D, 1> default_scissor = 
	{
		vk::Rect2D()
	};

	vk::PipelineViewportStateCreateInfo viewport_state(
		vk::PipelineViewportStateCreateFlags(),
		default_viewport,
		default_scissor);

	vk::PipelineRasterizationStateCreateInfo rasterization_state(
		vk::PipelineRasterizationStateCreateFlags(),
		in_create_info.rasterization_state.enable_depth_clamp,
		VK_FALSE,
		convert_polygon_mode(in_create_info.rasterization_state.polygon_mode),
		convert_cull_mode(in_create_info.rasterization_state.cull_mode),
		convert_front_face(in_create_info.rasterization_state.front_face),
		in_create_info.rasterization_state.enable_depth_bias,
		in_create_info.rasterization_state.depth_bias_constant_factor,
		in_create_info.rasterization_state.depth_bias_clamp,
		in_create_info.rasterization_state.depth_bias_slope_factor,
		1.0f);

	vk::PipelineMultisampleStateCreateInfo multisample_state(
		vk::PipelineMultisampleStateCreateFlags(),
		convert_sample_count_bit(in_create_info.multisampling_state.samples));

	vk::PipelineDepthStencilStateCreateInfo depth_stencil_state(
		vk::PipelineDepthStencilStateCreateFlags(),
		in_create_info.depth_stencil_state.enable_depth_test,
		in_create_info.depth_stencil_state.enable_depth_write,
		convert_compare_op(in_create_info.depth_stencil_state.depth_compare_op),
		in_create_info.depth_stencil_state.enable_depth_bounds_test,
		in_create_info.depth_stencil_state.enable_stencil_test,
		convert_stencil_op_state(in_create_info.depth_stencil_state.front_face),
		convert_stencil_op_state(in_create_info.depth_stencil_state.back_face));

	std::vector<vk::PipelineColorBlendAttachmentState> color_blend_attachments;
	color_blend_attachments.reserve(in_create_info.color_blend_state.attachment_states.size());
	for(size_t i = 0; i < render_pass->get_create_info().subpasses[in_create_info.subpass].color_attachments.size(); ++i)
	{
		color_blend_attachments.emplace_back(
			in_create_info.color_blend_state.attachment_states[i].enable_blend,
			convert_blend_factor(in_create_info.color_blend_state.attachment_states[i].src_color_blend_factor),
			convert_blend_factor(in_create_info.color_blend_state.attachment_states[i].dst_color_blend_factor),
			convert_blend_op(in_create_info.color_blend_state.attachment_states[i].color_blend_op),
			convert_blend_factor(in_create_info.color_blend_state.attachment_states[i].src_alpha_blend_factor),
			convert_blend_factor(in_create_info.color_blend_state.attachment_states[i].dst_alpha_blend_factor),
			convert_blend_op(in_create_info.color_blend_state.attachment_states[i].alpha_blend_op),
			convert_color_components_flags(in_create_info.color_blend_state.attachment_states[i].color_write_flags));
	}

	vk::PipelineColorBlendStateCreateInfo color_blend_state(
		vk::PipelineColorBlendStateCreateFlags(),
		in_create_info.color_blend_state.enable_logic_op,
		convert_logic_op(in_create_info.color_blend_state.logic_op),
		color_blend_attachments);

	std::array<vk::DynamicState, 2> dynamic_states = 
	{
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor,
	};

	vk::PipelineDynamicStateCreateInfo dynamic_state(
		vk::PipelineDynamicStateCreateFlags(),
		dynamic_states);

	auto [result, handle] = device.get_device().createGraphicsPipelineUnique(
		vk::PipelineCache(), vk::GraphicsPipelineCreateInfo(
			vk::PipelineCreateFlags(),
			shader_stages,
			&vertex_input_state,
			&input_assembly_state,
			nullptr,
			&viewport_state,
			&rasterization_state,
			&multisample_state,
			&depth_stencil_state,
			&color_blend_state,
			&dynamic_state,
			layout->get_layout(),
			render_pass->get_render_pass(),
			in_create_info.subpass));
	if(result != vk::Result::eSuccess)
		ze::logger::error("Failed to create gfx pipeline");

	pipeline = std::move(handle);
	last_pipeline_result = result;
}

Pipeline* Pipeline::get(const ResourceHandle& in_handle)
{
#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	auto pipeline = pipelines.find(in_handle);
	ZE_CHECKF(pipeline != pipelines.end(), "Invalid pipeline");
#endif	

	return get_resource<Pipeline>(in_handle);
}

}