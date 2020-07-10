#pragma once

#include "Module/Module.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Renderer/WorldView.h"
#include "Renderer/RendererTransientData.h"

namespace ZE::UI { class CImGuiRender; }
namespace ZE::JobSystem { struct SJob; }

namespace ZE::Renderer
{

struct SQuadVertex
{
	Math::SVector2f Position;
	Math::SVector2f TexCoord;

	SQuadVertex(const Math::SVector2f& InPosition,
		const Math::SVector2f& InTexCoord) : Position(InPosition), TexCoord(InTexCoord) {}
};

class CRendererModule : public CModule
{
public:
	virtual ~CRendererModule();

	RENDERER_API static CRendererModule& Get();

	RENDERER_API void Initialize() override;
	RENDERER_API void Destroy() override;

	RENDERER_API void CreateImGuiRenderer();

	/**
	 * Enqueue a view for rendering
	 */
	RENDERER_API void EnqueueView(const SWorldView& InView);

	/**
	 * Wait for rendering
	 */
	RENDERER_API void WaitRendering();

	/**
	 * Flush views
	 * Returns when game state copy is finished for all views
	 */
	RENDERER_API void FlushViews();

	RENDERER_API static inline CRSBufferPtr QuadVBuffer;
	RENDERER_API static inline CRSBufferPtr QuadIBuffer;

	RENDERER_API UI::CImGuiRender* GetImGuiRenderer() { return ImGuiRenderer.get(); }
private:
	void BeginDrawView(const SWorldView& InView);
private:
	/** Views to render */
	std::vector<SWorldView> Views;

	/** Rendering jobs to wait before flushing views */
	std::vector<const ZE::JobSystem::SJob*> RenderingJobs;

	TransientPerFrameDataMap TransientFrameDataMap;

	std::unique_ptr<UI::CImGuiRender> ImGuiRenderer;
};

}