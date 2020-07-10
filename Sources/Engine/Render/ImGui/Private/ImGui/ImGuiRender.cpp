#include "ImGui/ImGuiRender.h"
#include "ImGui/ImGui.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/RenderSystem/RenderSystemContext.h"
#include "Render/Shader/BasicShader.h"
#include "Module/ModuleManager.h"

DECLARE_LOG_CATEGORY(ImGuiRender);

namespace ZE::UI
{

class CImGuiVS : public CBasicShader
{
	DECLARE_SHADER(CImGuiVS, CBasicShader)

public:
	CImGuiVS(const CShaderType* InType, const SShaderCompilerOutput& InOutput) 
		: CBasicShader(InType, InOutput) {}
};

class CImGuiFS : public CBasicShader
{
	DECLARE_SHADER(CImGuiFS, CBasicShader)

public:
	CImGuiFS(const CShaderType* InType, const SShaderCompilerOutput& InOutput) 
		: CBasicShader(InType, InOutput) {}
};

IMPLEMENT_SHADER(CImGuiVS, "ImGuiVS", 
	"/UI/ImGuiVS.hlsl", "Main", EShaderStage::Vertex);

IMPLEMENT_SHADER(CImGuiFS, "ImGuiFS",
	"/UI/ImGuiFS.hlsl", "Main", EShaderStage::Fragment);

struct SGlobalData
{
	Math::SVector2f Scale;
	Math::SVector2f Translate;
};

CImGuiRender::CImGuiRender()
{
	ImGuiIO& IO = ImGui::GetIO();

	/**
	 * Create font texture
	 */
	uint8_t* FontData = nullptr;
	int FontWidth = 0;
	int FontHeight = 0;

	IO.Fonts->GetTexDataAsRGBA32(&FontData, &FontWidth, &FontHeight);
	Font = GRenderSystem->CreateTexture(
		ERSTextureType::Tex2D,
		ERSTextureUsage::Sampled,
		ERSMemoryUsage::DeviceLocal,
		EFormat::R8G8B8A8UNorm,
		FontWidth,
		FontHeight,
		1,
		1,
		1,
		ESampleCount::Sample1,
		SRSResourceCreateInfo(FontData, "ImGuiRender Font"));
	if(!Font)
	{
		LOG(ELogSeverity::Error, ImGuiRender, "Failed to create ImGui font, will not render");
		return;
	}

	/** UBO */
	GlobalData = GRenderSystem->CreateBuffer(
		ERSBufferUsage::UniformBuffer,
		ERSMemoryUsage::HostVisible | ERSMemoryUsage::UsePersistentMapping,
		sizeof(SGlobalData),
		SRSResourceCreateInfo(nullptr, "ImGuiRender Global Data UBO"));
	if(!GlobalData)
	{
		LOG(ELogSeverity::Error, ImGuiRender, "Failed to create ImGui ubo, will not render");
		return;
	}

	/** Sampler */
	Sampler = GRenderSystem->CreateSampler(SRSSamplerCreateInfo());
	if (!Sampler)
	{
		LOG(ELogSeverity::Error, ImGuiRender, "Failed to create ImGui sampler, will not render");
		return;
	}

	/**
	 * Pipeline configuration
	 */
	Pipeline.ShaderStages.emplace_back(EShaderStage::Vertex,
		CBasicShaderManager::Get().GetShader("ImGuiVS")->GetShader(), "Main");
	Pipeline.ShaderStages.emplace_back(EShaderStage::Fragment,
		CBasicShaderManager::Get().GetShader("ImGuiFS")->GetShader(), "Main");
	Pipeline.AttributeDescriptions = 
	{
		SVertexInputAttributeDescription(0, 0, EFormat::R32G32Sfloat, offsetof(ImDrawVert, pos)),
		SVertexInputAttributeDescription(0, 1, EFormat::R32G32Sfloat, offsetof(ImDrawVert, uv)),
		SVertexInputAttributeDescription(0, 2, EFormat::R8G8B8A8UNorm, offsetof(ImDrawVert, col)),
	};
	Pipeline.BindingDescriptions = 
	{
		SVertexInputBindingDescription(0, sizeof(ImDrawVert), EVertexInputRate::Vertex)
	};
	Pipeline.RasterizerState.CullMode = ECullMode::None;
	Pipeline.RasterizerState.FrontFace = EFrontFace::CounterClockwise;
	Pipeline.BlendState.BlendDescs = 
	{
		SRSRenderTargetBlendDesc(
			true,
			EBlendFactor::SrcAlpha,
			EBlendFactor::OneMinusSrcAlpha,
			EBlendOp::Add,
			EBlendFactor::OneMinusSrcAlpha,
			EBlendFactor::Zero,
			EBlendOp::Add)
	};

	DrawData = std::make_unique<ImDrawData>();
}

void CImGuiRender::CopyDrawdata()
{
	ImDrawData* ImGuiDrawData = ImGui::GetDrawData();
	if(!ImGuiDrawData)
		return;

	bool bRecreateCmdList = ImGuiDrawData->CmdListsCount != DrawData->CmdListsCount;
	ImDrawList** DrawList = DrawData->CmdLists;

	for (size_t i = 0; i < DrawData->CmdListsCount; ++i)
	{
		IM_DELETE(DrawData->CmdLists[i]);
	}
	
	if(bRecreateCmdList)
		delete DrawData->CmdLists;

	memcpy(DrawData.get(), ImGuiDrawData, sizeof(ImDrawData));
	DrawData->CmdLists = DrawList;

	if(bRecreateCmdList)
		DrawData->CmdLists = new ImDrawList*[DrawData->CmdListsCount];

	for(size_t i = 0; i < DrawData->CmdListsCount; ++i)
	{
		DrawData->CmdLists[i] = ImGuiDrawData->CmdLists[i]->CloneOutput();
	}
}

void CImGuiRender::Update()
{
	if (!DrawData)
		return;

	uint64_t VertexSize = DrawData->TotalVtxCount * sizeof(ImDrawVert);
	uint64_t IndexSize = DrawData->TotalIdxCount * sizeof(ImDrawIdx);

	if (VertexSize == 0 || IndexSize == 0)
		return;

	/** Recreate buffers if modified and needed */
	if (!VertexBuffer || LastVertexSize < VertexSize)
	{
		VertexBuffer = GRenderSystem->CreateBuffer(
			ERSBufferUsage::VertexBuffer,
			ERSMemoryUsage::HostVisible,
			VertexSize,
			SRSResourceCreateInfo(nullptr, "ImGuiVertexBuffer"));
		if (!VertexBuffer)
		{
			LOG(ELogSeverity::Error, ImGuiRender, "Failed to create ImGui vertex buffer");
			return;
		}

		VertexCount = DrawData->TotalVtxCount;
		LastVertexSize = VertexSize;
	}

	if (!IndexBuffer || LastIndexSize < IndexSize)
	{
		IndexBuffer = GRenderSystem->CreateBuffer(
			ERSBufferUsage::IndexBuffer,
			ERSMemoryUsage::HostVisible,
			IndexSize,
			SRSResourceCreateInfo(nullptr, "ImGuiIndexBuffer"));
		if (!IndexBuffer)
		{
			LOG(ELogSeverity::Error, ImGuiRender, "Failed to create ImGui index buffer");
			return;
		}

		IndexCount = DrawData->TotalIdxCount;
		LastIndexSize = IndexSize;
	}

	/** Write data to both buffers */
	ImDrawVert* VertexData = reinterpret_cast<ImDrawVert*>(VertexBuffer->Map(ERSBufferMapMode::WriteOnly));
	ImDrawIdx* IndexData = reinterpret_cast<ImDrawIdx*>(IndexBuffer->Map(ERSBufferMapMode::WriteOnly));

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

void CImGuiRender::Draw(IRenderSystemContext* InContext)
{
	if (!DrawData || !Sampler || !GlobalData || !Font || !VertexBuffer || !IndexBuffer)
		return;

	ImGuiIO& IO = ImGui::GetIO();

	/** Update UBO */
	SGlobalData GD;
	GD.Scale = Math::SVector2f(2.0f / IO.DisplaySize.x, 2.0f / IO.DisplaySize.y);
	GD.Translate = Math::SVector2f(-1.0f);
	memcpy(GlobalData->GetMappedData(), &GD, sizeof(GD));

	/** Render */
	InContext->BindGraphicsPipeline(Pipeline);
	InContext->SetShaderUniformBuffer(0, 0, GlobalData.get());
	InContext->SetShaderTexture(0, 1, Font.get());
	InContext->SetShaderSampler(0, 2, Sampler.get());

	uint32_t VertexOffset = 0;
	uint32_t IndexOffset = 0;
	uint64_t VertexSize = DrawData->TotalVtxCount * sizeof(ImDrawVert);
	uint64_t IndexSize = DrawData->TotalIdxCount * sizeof(ImDrawIdx);

	if (DrawData->CmdListsCount > 0 &&
		LastVertexSize >= VertexSize &&
		LastIndexSize >= IndexSize)
	{
		InContext->BindVertexBuffers({ VertexBuffer.get() });
		InContext->BindIndexBuffer(IndexBuffer.get(), 0, EIndexFormat::Uint16);
		for (int32_t i = 0; i < DrawData->CmdListsCount; i++)
		{
			ImDrawList* CmdList = DrawData->CmdLists[i];
			for (int32_t j = 0; j < CmdList->CmdBuffer.Size; j++)
			{
				ImDrawCmd* Cmd = &CmdList->CmdBuffer[j];

				//Math::SRect2D Scissor;
				//Scissor.Position.x = std::max(Cmd->ClipRect.x, 0.f);
				//Scissor.Position.y = std::max(Cmd->ClipRect.y, 0.f);
				//Scissor.Size.x = Cmd->ClipRect.z - Cmd->ClipRect.x;
				//Scissor.Size.y = Cmd->ClipRect.w - Cmd->ClipRect.y;

				//InContext->SetScissors({ Scissor });

				InContext->DrawIndexed(Cmd->ElemCount,
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

}