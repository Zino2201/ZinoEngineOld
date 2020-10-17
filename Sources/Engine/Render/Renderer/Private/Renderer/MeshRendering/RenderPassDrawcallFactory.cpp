#include "Renderer/MeshRendering/RenderPassDrawcallFactory.h"
#include "Renderer/MeshRendering/MeshDrawcall.h"
#include "Renderer/Mesh.h"

namespace ze::renderer
{

CRenderPassDrawcallFactory::CRenderPassDrawcallFactory(CWorldProxy& InWorld)
	: World(InWorld) {}

void CRenderPassDrawcallFactory::ComputeDrawcalls(const SMesh& InMesh,
	const SRSGraphicsPipeline& InPipeline,
	const std::vector<SMeshDrawcallShaderBinding>& InBindings)
{
	size_t DrawcallIdx = DrawcallList->Emplace(std::move(InPipeline),
		InMesh.VertexBuffer,
		InMesh.IndexBuffer,
		InMesh.IndexFormat,
		InMesh.VertexCount == 0 ? InMesh.IndexCount : InMesh.VertexCount,
		1,
		0,
		0);

	std::move(InBindings.begin(), InBindings.end(), DrawcallList->At(DrawcallIdx).Bindings.begin());
}

/** Mgr */

namespace RenderPassDrawcallFactory
{

void RegisterDrawcallFactory(const ERenderPassFlagBits& InRenderPass,
	IRenderPassDrawcallFactoryCreator* Creator)
{
	IRenderPassDrawcallFactoryCreator::Creators[InRenderPass] = Creator;
}

IRenderPassDrawcallFactoryCreator* GetCreatorForRenderPass(const ERenderPassFlagBits& InRenderPass)
{
	return IRenderPassDrawcallFactoryCreator::Creators[InRenderPass];
}

const robin_hood::unordered_map<ERenderPassFlagBits, IRenderPassDrawcallFactoryCreator*>& GetCreators()
{
	return IRenderPassDrawcallFactoryCreator::Creators;
}

}

}