#include "ImGuiRender.h"
#include "imgui.h"
#include "Core/Engine.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/Commands/RenderCommandContext.h"

struct SImGuiUBO
{
	glm::vec2 Scale;
	glm::vec2 Translate;
};

std::vector<SVertexInputBindingDescription> GetBindingDescriptions()
{
	return { SVertexInputBindingDescription(0, sizeof(ImDrawVert), EVertexInputRate::Vertex) };
}

std::vector<SVertexInputAttributeDescription> GetAttributeDescriptions()
{
	return
	{
		SVertexInputAttributeDescription(0, 0, EFormat::R32G32Sfloat,
			offsetof(ImDrawVert, pos)),
		SVertexInputAttributeDescription(0, 1, EFormat::R32G32Sfloat,
			offsetof(ImDrawVert, uv)),
		SVertexInputAttributeDescription(0, 2, EFormat::R8G8B8A8UNorm,
			offsetof(ImDrawVert, col)),
	};
}

void CImGuiRender::InitRenderThread()
{
	ImGuiIO& IO = ImGui::GetIO();

	/** Create font texture */
	uint8_t* FontData;
	int Width, Height;
	IO.Fonts->GetTexDataAsRGBA32(&FontData, &Width, &Height);

	Font.Init(static_cast<uint32_t>(Width), static_cast<uint32_t>(Height), 4, FontData);
	Font.InitResources();

	ShaderMap.AddShader(CBasicShaderClass::BasicShaderMap["ImGuiShaderVS"].get());
	ShaderMap.AddShader(CBasicShaderClass::BasicShaderMap["ImGuiShaderFS"].get());

	/** Create pipeline */
	Pipeline = g_Engine->GetRenderSystem()->CreateGraphicsPipeline(
		SRenderSystemGraphicsPipelineInfos(
		ShaderMap.GetShader(EShaderStage::Vertex)->GetShader(),
		ShaderMap.GetShader(EShaderStage::Fragment)->GetShader(),
		GetBindingDescriptions(),
		GetAttributeDescriptions(),
		ShaderMap.GetParameters(),
		SRenderSystemRasterizerState(
			false,
			false,
			EPolygonMode::Fill,
			ECullMode::None,
			EFrontFace::CounterClockwise),
		{ 
			true,
			EBlendFactor::SrcAlpha,
			EBlendFactor::OneMinusSrcAlpha,
			EBlendOp::Add,
			EBlendFactor::OneMinusSrcAlpha,
			EBlendFactor::Zero,
			EBlendOp::Add
		}));

	UniformBuffer = g_Engine->GetRenderSystem()->CreateUniformBuffer(
		SRenderSystemUniformBufferInfos(sizeof(SImGuiUBO)));
}

void CImGuiRender::DestroyRenderThread()
{
	Font.DestroyResources();
	VertexBuffer->Destroy();
	IndexBuffer->Destroy();
	UniformBuffer->Destroy();
}

void CImGuiRender::UpdateBuffers()
{
	ImDrawData* DrawData = ImGui::GetDrawData();

	if(!DrawData)
		return;

	uint64_t VertexSize = DrawData->TotalVtxCount * sizeof(ImDrawVert);
	uint64_t IndexSize = DrawData->TotalIdxCount * sizeof(ImDrawIdx);

	if (VertexSize == 0 || IndexSize == 0)
		return;

	/** Recreate buffers if modified */
	if(!VertexBuffer || LastVertexSize < VertexSize)
	{
		if (VertexBuffer)
			VertexBuffer->Destroy();

		VertexBuffer = g_Engine->GetRenderSystem()->CreateVertexBuffer(VertexSize,
			EBufferMemoryUsage::CpuToGpu,
			false,
			"ImGuiVertexBuffer");

		VertexCount = DrawData->TotalVtxCount;
		LastVertexSize = VertexSize;
	}

	if (!IndexBuffer || LastIndexSize < IndexSize)
	{
		if (IndexBuffer)
			IndexBuffer->Destroy();
		
		IndexBuffer = g_Engine->GetRenderSystem()->CreateIndexBuffer(IndexSize,
			EBufferMemoryUsage::CpuToGpu,
			false,
			"ImGuiIndexBuffer");

		IndexCount = DrawData->TotalIdxCount;
		LastIndexSize = IndexSize;
	}

	/** Write data to both buffers */
	ImDrawVert* VertexData = reinterpret_cast<ImDrawVert*>(VertexBuffer->Map());
	ImDrawIdx* IndexData = reinterpret_cast<ImDrawIdx*>(IndexBuffer->Map());

	for (int i = 0; i < DrawData->CmdListsCount; i++)
	{
		ImDrawList* CmdList = DrawData->CmdLists[i];
		memcpy(VertexData, CmdList->VtxBuffer.Data, CmdList->VtxBuffer.Size * sizeof(ImDrawVert));
		memcpy(IndexData, CmdList->IdxBuffer.Data, CmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
		VertexData += CmdList->VtxBuffer.Size;
		IndexData += CmdList->IdxBuffer.Size;
	}

	VertexBuffer->Unmap();
	IndexBuffer->Unmap();
}

void CImGuiRender::Draw(IRenderCommandContext* InCommandContext)
{
	must(IsInRenderThread());

	if(!VertexBuffer || !IndexBuffer)
		return;

	ImGuiIO& IO = ImGui::GetIO();

	InCommandContext->BindGraphicsPipeline(Pipeline.get());

	// TODO: Move

	SImGuiUBO UBO;
	UBO.Scale = glm::vec2(2.0f / IO.DisplaySize.x, 2.0f / IO.DisplaySize.y);
	UBO.Translate = glm::vec2(-1.0f);
	memcpy(UniformBuffer->GetMappedMemory(), &UBO, sizeof(UBO));

	InCommandContext->SetShaderUniformBuffer(0, 0, UniformBuffer.get());
	InCommandContext->SetShaderCombinedImageSampler(0, 1, Font.GetTextureView());

	ImDrawData* DrawData = ImGui::GetDrawData();
	uint32_t VertexOffset = 0;
	uint32_t IndexOffset = 0;
	uint64_t VertexSize = DrawData->TotalVtxCount * sizeof(ImDrawVert);
	uint64_t IndexSize = DrawData->TotalIdxCount * sizeof(ImDrawIdx);

	if (DrawData->CmdListsCount > 0 &&
		 LastVertexSize >= VertexSize &&
		 LastIndexSize >= IndexSize) 
	{
		InCommandContext->BindVertexBuffers({ VertexBuffer.get() });
		InCommandContext->BindIndexBuffer(IndexBuffer.get(), 0, EIndexFormat::Uint16);
		for (int32_t i = 0; i < DrawData->CmdListsCount; i++)
		{
			ImDrawList* CmdList = DrawData->CmdLists[i];
			for (int32_t j = 0; j < CmdList->CmdBuffer.Size; j++)
			{
				ImDrawCmd* Cmd = &CmdList->CmdBuffer[j];

				SRect2D Scissor;
				Scissor.Position.x = std::max(Cmd->ClipRect.x, 0.f);
				Scissor.Position.y = std::max(Cmd->ClipRect.y, 0.f);
				Scissor.Size.x = Cmd->ClipRect.z - Cmd->ClipRect.x;
				Scissor.Size.y = Cmd->ClipRect.w - Cmd->ClipRect.y;

				InCommandContext->SetScissors({ Scissor });

				InCommandContext->DrawIndexed(Cmd->ElemCount,
					1,
					Cmd->IdxOffset + IndexOffset,
					Cmd->VtxOffset + VertexOffset,
					0);

			}

			VertexOffset += CmdList->VtxBuffer.Size;
			IndexOffset += CmdList->IdxBuffer.Size;
		}
	}
}

CImGuiShaderVS::CImGuiShaderVS(CShaderClass* InClass,
	const SCompiledShaderData& InData) : CBasicShader(InClass, InData) {}

CImGuiShaderFS::CImGuiShaderFS(CShaderClass* InClass,
	const SCompiledShaderData& InData) : CBasicShader(InClass, InData) {}

DECLARE_SHADER_CLASS(CImGuiShaderVS, "ImGuiShaderVS", "Assets/Shaders/Basic/ImGui.vert",
	EShaderStage::Vertex)

DECLARE_SHADER_CLASS(CImGuiShaderFS, "ImGuiShaderFS", "Assets/Shaders/Basic/ImGui.frag",
	EShaderStage::Fragment)
