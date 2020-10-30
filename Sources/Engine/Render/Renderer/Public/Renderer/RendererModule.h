#pragma once

#include "Module/Module.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Renderer/WorldView.h"
#include "Renderer/RendererTransientData.h"

namespace ze::ui { class CImGuiRender; }
namespace ze::jobsystem { struct Job; }

namespace ze::renderer
{
	
class CWorldRenderer;

struct SQuadVertex
{
	maths::Vector2f Position;
	maths::Vector2f TexCoord;

	SQuadVertex(const maths::Vector2f& InPosition,
		const maths::Vector2f& InTexCoord) : Position(InPosition), TexCoord(InTexCoord) {}
};

class CRendererModule : public ze::module::Module
{
public:
	RENDERER_API CRendererModule();
	RENDERER_API virtual ~CRendererModule();

	RENDERER_API static CRendererModule& Get();

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
private:
	void BeginDrawView(const SWorldView& InView);
private:
	/** Views to render */
	std::vector<SWorldView> Views;

	/** Rendering jobs to wait before flushing views */
	std::vector<const ze::jobsystem::Job*> RenderingJobs;

	TransientPerFrameDataMap TransientFrameDataMap;
	std::vector<std::unique_ptr<CWorldRenderer>> WorldRenderers;
};

}