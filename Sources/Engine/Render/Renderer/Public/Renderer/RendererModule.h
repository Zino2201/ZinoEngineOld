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
	static CRendererModule& Get()
	{
		static CRendererModule& Module = *CModuleManager::LoadModule<CRendererModule>("Renderer");
		return Module;
	}

	void Initialize() override;
	void Destroy() override;

	void CreateImGuiRenderer();

	/**
	 * Enqueue a view for rendering
	 */
	void EnqueueView(const SWorldView& InView);

	/**
	 * Wait for rendering
	 */
	void WaitRendering();

	/**
	 * Flush views
	 * Returns when game state copy is finished for all views
	 */
	void FlushViews();

	static inline CRSBufferPtr QuadVBuffer;
	static inline CRSBufferPtr QuadIBuffer;

	UI::CImGuiRender* GetImGuiRenderer() { return ImGuiRenderer.get(); }
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