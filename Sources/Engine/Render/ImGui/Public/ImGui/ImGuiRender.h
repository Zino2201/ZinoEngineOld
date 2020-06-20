#pragma once

#include "Render/RenderThreadResource.h"
#include "Render/RenderSystem/RenderSystemResources.h"

namespace ZE { class IRenderSystemContext; }

namespace ZE::UI
{

/**
 * ImGui render object to render imgui uis
 */
class IMGUI_API CImGuiRender : public CRenderThreadResource
{
public:
	void InitResource_RenderThread() override;
	void DestroyResource_RenderThread() override;

	/**
	 * Update buffers
	 */
	void Update();
	void Draw(IRenderSystemContext* InContext);
private:
	SRSGraphicsPipeline Pipeline;
	CRSTexturePtr Font;
	CRSBufferPtr VertexBuffer;
	CRSBufferPtr IndexBuffer;
	CRSBufferPtr GlobalData;
	CRSSamplerPtr Sampler;
	uint64_t VertexCount;
	uint64_t IndexCount;
	uint64_t LastVertexSize;
	uint64_t LastIndexSize;
};

}