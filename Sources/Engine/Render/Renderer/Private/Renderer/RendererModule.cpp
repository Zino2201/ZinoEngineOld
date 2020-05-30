#include "Renderer/RendererModule.h"
#include "Renderer/FrameGraph/RenderPass.h"
#include "Render/RenderSystem/RenderSystem.h"
#include <array>

DEFINE_MODULE(ZE::Renderer::CRendererModule, Renderer);

namespace ZE::Renderer
{

const std::array<SQuadVertex, 4> QuadVertices =
{
	SQuadVertex(Math::SVector2f(-1, -1), Math::SVector2f(0, 0)),
	SQuadVertex(Math::SVector2f(1, -1), Math::SVector2f(1, 0)),
	SQuadVertex(Math::SVector2f(1, 1), Math::SVector2f(1, 1)),
	SQuadVertex(Math::SVector2f(-1, 1), Math::SVector2f(0, 1)),
};

const std::array<uint16_t, 6> QuadIndices = 
{
	 0, 1, 2, 
	 2, 3, 0
};

void CRendererModule::Initialize()
{
	QuadVBuffer = GRenderSystem->CreateBuffer(
		ERSBufferUsage::VertexBuffer,
		ERSMemoryUsage::DeviceLocal,
		QuadVertices.size() * sizeof(QuadVertices.front()),
		SRSResourceCreateInfo(QuadVertices.data()));

	QuadIBuffer = GRenderSystem->CreateBuffer(
		ERSBufferUsage::IndexBuffer,
		ERSMemoryUsage::DeviceLocal,
		QuadIndices.size() * sizeof(QuadIndices.front()),
		SRSResourceCreateInfo(QuadIndices.data()));
}

void CRendererModule::Destroy()
{
	QuadVBuffer.reset();
	QuadIBuffer.reset();
	CRenderPassPersistentResourceManager::Get().Destroy();
}

}