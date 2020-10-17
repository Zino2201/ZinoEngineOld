#pragma once

#include "RenderPass.h"
#include "Renderer/MeshRendering/MeshDrawcall.h"

namespace ze { class IRenderSystemContext; }
namespace ze::jobsystem { struct Job; }

namespace ze::renderer
{

class CWorldProxy;
class CRenderPassDrawcallFactory;
struct SWorldView;

/**
 * Class that manage the rendering of a render pass
 * It instantiate mesh drawcalls and draw them later
 * If this render pass is static (defined in ERenderPass), static drawcalls are cached and reused
 */
class CRenderPassRenderer
{
public:
	CRenderPassRenderer(CWorldProxy& InProxy);

	/**
	 * Prepare drawcalls (parallel)
	 */
	void PrepareDrawcalls(SWorldView& InView, const ERenderPassFlagBits& InRenderPass,
		CRenderPassDrawcallFactory& InDrawcallFactory);
	void Draw(IRenderSystemContext& InContext) const;
private:
	void SubmitDrawcall(IRenderSystemContext& InContext, const SMeshDrawcall& InDrawcall) const;
private:
	CWorldProxy& WorldProxy;

	/** Storage for dynamic drawcalls */
	TDrawcallList DynamicDrawcalls;

	/** Final sorted drawcall list */
	TTransientDrawcallRefList FinalDrawcalls;

	const jobsystem::Job* PrepareJob;
};

}