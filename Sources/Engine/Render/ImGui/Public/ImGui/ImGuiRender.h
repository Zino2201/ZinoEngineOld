#pragma once

#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/RenderSystem/Resources/GraphicsPipeline.h"
#include "ImGui/ImGui.h"

namespace ze { class IRenderSystemContext; }

namespace ze::ui
{

/**
 * ImGui render object to render imgui uis
 */
class IMGUI_API CImGuiRender
{
public:
	CImGuiRender();
	~CImGuiRender();

	/**
	 * Update buffers
	 */
	void CopyDrawdata();
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
	std::unique_ptr<ImDrawData> DrawData;
};

}