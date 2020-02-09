#include "ImGui.h"
//#include "ThirdParty/imgui.h"
#include "Core/Engine.h"
#include "Render/Window.h"
//#include "Render/Material.h"
#include "Core/AssetManager.h"

CImGui::CImGui()
{
	//ImGui::CreateContext();

	///** Init styles */
	//ImGuiStyle& Style = ImGui::GetStyle();
	//Style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
	//Style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
	//Style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
	//Style.Colors[ImGuiCol_Header] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
	//Style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);

	///** Dimensions */
	//ImGuiIO& IO = ImGui::GetIO();
	//IO.DisplaySize = ImVec2(CEngine::Get().GetWindow()->GetWidth(), 
	//	CEngine::Get().GetWindow()->GetHeight());
	//IO.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

	///** Resources */

	///** Font texture, sampler, view */
	//{
	//	unsigned char* FontData;
	//	int TexWidth, TexHeight;
	//	IO.Fonts->GetTexDataAsRGBA32(&FontData, &TexWidth, &TexHeight);
	//	uint64_t Size = TexWidth * TexHeight * 4 * sizeof(char);

	//	/** Staging buffer */
	//	std::shared_ptr<IBuffer> StagingBuffer = CEngine::Get().GetRenderSystem()->CreateBuffer(
	//		SBufferInfos(Size, EBufferUsage::TransferSrc, EBufferMemoryUsage::CpuToGpu));

	//	/** Copy pixels to buffer */
	//	void* StagingData = StagingBuffer->Map();
	//	memcpy(StagingData, FontData, static_cast<size_t>(Size));
	//	StagingBuffer->Unmap();

	//	/** Texture */
	//	Texture = CEngine::Get().GetRenderSystem()->CreateTexture(
	//		STextureInfo(ETextureType::Texture2D, EFormat::R8G8B8A8UNorm,
	//			ETextureUsage::TransferDst | ETextureUsage::Sampled,
	//			ETextureMemoryUsage::GpuOnly,
	//			TexWidth,
	//			TexHeight,
	//			1,
	//			0));

	//	/** Copy buffer to texture */
	//	Texture->Copy(StagingBuffer.get());

	//	/** Create texture view */
	//	TextureView = CEngine::Get().GetRenderSystem()->CreateTextureView(
	//		STextureViewInfo(Texture.get(), ETextureViewType::ShaderResource, EFormat::R8G8B8A8UNorm,
	//			MipLevels));

	//	/** Create a sampler */
	//	Sampler = CEngine::Get().GetRenderSystem()->CreateSampler(SSamplerInfo(
	//		ESamplerFilter::Linear,
	//		ESamplerFilter::Linear,
	//		ESamplerFilter::Linear,
	//		ESamplerAddressMode::Clamp,
	//		ESamplerAddressMode::Clamp,
	//		ESamplerAddressMode::Clamp,
	//		16.f,
	//		EComparisonOp::Always,
	//		0.f,
	//		0.f,
	//		0.f));

	//	TextureView->SetSampler(Sampler);
	//}

	//UIMaterial = CEngine::Get().GetAssetManager()->Get<CMaterial>("Materials/ui.json");
}

CImGui::~CImGui()
{
	//ImGui::DestroyContext();
}