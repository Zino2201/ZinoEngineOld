#include "Renderer/MeshRendering/RenderPassDrawcallFactory.h"
#include "Renderer/MeshRendering/MeshDrawcall.h"
#include "Renderer/Mesh.h"

namespace ZE::Renderer
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
		InMesh.VertexCount,
		InMesh.IndexCount,
		1,
		0,
		0,
		0);

	std::move(InBindings.begin(), InBindings.end(), DrawcallList->At(DrawcallIdx).Bindings.begin());
}

/** Mgr */

namespace RenderPassDrawcallFactory
{
static robin_hood::unordered_map<ERenderPass, IRenderPassDrawcallFactoryCreator*> Creators;

void RegisterDrawcallFactory(const ERenderPass& InRenderPass,
	IRenderPassDrawcallFactoryCreator* Creator)
{
	Creators[InRenderPass] = Creator;
}

IRenderPassDrawcallFactoryCreator* GetCreatorForRenderPass(const ERenderPass& InRenderPass)
{
	return Creators[InRenderPass];
}

const robin_hood::unordered_map<ERenderPass, IRenderPassDrawcallFactoryCreator*>& GetCreators()
{
	return Creators;
}

}

}