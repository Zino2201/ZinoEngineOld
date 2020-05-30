#pragma once

#include "EngineCore.h"
#include <array>

namespace ZE::Renderer
{

class CWorldProxy;
class CRenderableComponentProxy;
struct SStaticProxyData;
class CMeshCollection;

/**
 * Render Pass
 */
enum class EMeshRenderPass
{
    None = 1 << 0,
    BasePass = 1 << 1,
};

constexpr std::array<EMeshRenderPass, 1> GMeshRenderPasses = { EMeshRenderPass::BasePass };

DECLARE_FLAG_ENUM(EMeshRenderPass);

/**
 * Base abstract class for mesh render passes
 */
class CMeshRenderPass
{
    template<typename Vertex, typename Fragment>
    struct SShaderArray
    {
        boost::intrusive_ptr<Vertex> VertexShader;
        boost::intrusive_ptr<Fragment> FragmentShader;
    };
public:
    CMeshRenderPass(EMeshRenderPass InRenderPass);

	virtual void Process(CWorldProxy* InWorldProxy,
		CRenderableComponentProxy* InProxy,
		const CMeshCollection& InCollection,
        const size_t& InInstanceIdx) = 0;
protected:
    template<typename ShaderArray>
    void BuildDrawCommand(CWorldProxy* InWorldProxy,
        CRenderableComponentProxy* InProxy,
	    const CMeshCollection& InCollection,
		const size_t& InInstanceIdx,
        const ShaderArray& InShaderArray);
public:
    static CMeshRenderPass* GetMeshPass(EMeshRenderPass InRenderPass)
    {
        return RenderPasses[InRenderPass];
    }
private:
    inline static std::unordered_map<EMeshRenderPass, CMeshRenderPass*> RenderPasses;
    EMeshRenderPass RenderPass;
};

#define IMPLEMENT_MESH_RENDER_PASS(Class, Pass) static Class MeshRenderPass_##Class(Pass)

}