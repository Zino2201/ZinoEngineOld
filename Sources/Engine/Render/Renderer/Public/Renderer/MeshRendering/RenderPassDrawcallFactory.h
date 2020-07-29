#pragma once

#include "Render/RenderCore.h"
#include "Renderer/MeshRendering/MeshDrawcall.h"
#include "Renderer/RenderPass/RenderPass.h"
#include <robin_hood.h>

namespace ZE { class CRSShader; }

namespace ZE::Renderer
{

class CWorldProxy;
class CRenderableComponentProxy;
struct SMesh;
struct SWorldView;

/**
 * Base abstract class for render pass drawcall factories that
 * turn meshes into drawcalls
 */
class CRenderPassDrawcallFactory
{
public:
	CRenderPassDrawcallFactory(CWorldProxy& InWorld);

	/**
	 * Process the specified mesh of the specified proxy and generate drawcalls
	 * for this pass
	 */
	virtual void ProcessMesh(const SWorldView& InWorldView,
		const CRenderableComponentProxy& InProxy,
		const SMesh& InMesh) = 0;

	void SetDrawcallList(TDrawcallList* InList) { DrawcallList = InList; }
protected:
	void ComputeDrawcalls(const SMesh& InMesh,
		const SRSGraphicsPipeline& InPipeline,
		const std::vector<SMeshDrawcallShaderBinding>& InBindings);
private:
	CWorldProxy& World;
	TDrawcallList* DrawcallList;
};

/**
 * Utility namespace to instantiate drawcall factories for specific passes
 */
namespace RenderPassDrawcallFactory 
{

struct IRenderPassDrawcallFactoryCreator 
{ 
	virtual TOwnerPtr<CRenderPassDrawcallFactory> Instantiate(CWorldProxy& InWorld) = 0; 
	virtual ERenderPassFlagBits GetRenderPass() const = 0;

	inline static robin_hood::unordered_map<ERenderPassFlagBits, IRenderPassDrawcallFactoryCreator*> Creators;
};

void RegisterDrawcallFactory(const ERenderPassFlagBits& InRenderPass,
	IRenderPassDrawcallFactoryCreator* Creator);
IRenderPassDrawcallFactoryCreator* GetCreatorForRenderPass(const ERenderPassFlagBits& InRenderPass);
const robin_hood::unordered_map<ERenderPassFlagBits, IRenderPassDrawcallFactoryCreator*>& GetCreators();

/**
 * Helper struct
 */
template<ERenderPassFlagBits RenderPass, typename T>
struct TRenderPassDrawcallFactoryCreator : public IRenderPassDrawcallFactoryCreator
{
	TRenderPassDrawcallFactoryCreator()
	{
		RegisterDrawcallFactory(RenderPass, this);
	}

	TOwnerPtr<CRenderPassDrawcallFactory> Instantiate(CWorldProxy& InWorld) override
	{
		return new T(InWorld);
	}

	ERenderPassFlagBits GetRenderPass() const override
	{
		return RenderPass;
	}
};

}

#define DEFINE_RENDER_PASS_DRAWCALL_FACTORY(RenderPass, FactoryClass) ZE::Renderer::RenderPassDrawcallFactory::TRenderPassDrawcallFactoryCreator<RenderPass, FactoryClass> G##FactoryClass;

}