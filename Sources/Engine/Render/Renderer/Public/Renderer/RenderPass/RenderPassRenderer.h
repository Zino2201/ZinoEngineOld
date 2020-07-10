#pragma once

#include "RenderPass.h"
#include "Renderer/MeshRendering/MeshDrawcall.h"

namespace ZE { class IRenderSystemContext; }
namespace ZE::JobSystem { struct SJob; }

namespace ZE::Renderer
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
	void PrepareDrawcalls(SWorldView& InView, const ERenderPass& InRenderPass,
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

	const JobSystem::SJob* PrepareJob;
};

}