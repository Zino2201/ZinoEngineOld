#include "Renderer/MeshRenderPass.h"

namespace ZE::Renderer
{

CMeshRenderPass::CMeshRenderPass(EMeshRenderPass InRenderPass)
	: RenderPass(InRenderPass)
{
	RenderPasses.insert( { InRenderPass, this } );
}

}