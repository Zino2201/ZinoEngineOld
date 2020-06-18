#include "Engine/Components/StaticMeshComponent.h"
#include "Reflection/Builders.h"
#include "Engine/Assets/StaticMesh.h"

namespace ZE::Components
{

/** Component */
void SStaticMeshComponent::SetStaticMesh(const std::shared_ptr<CStaticMesh>& InSM)
{
	StaticMesh = InSM;
	ECS::CECSManager::Get().GetSystem<CRenderableComponentSystem>()
		->AddRenderableComponentToUpdate(this);
}

/**
 * Proxy for a static mesh component
 */
class CStaticMeshComponentProxy : public Renderer::CRenderableComponentProxy
{
public:
    CStaticMeshComponentProxy(Renderer::CWorldProxy* InWorld,
		const SStaticMeshComponent* InStaticMesh) : Renderer::CRenderableComponentProxy(InWorld,
			Renderer::ERenderableComponentProxyCacheMode::Cachable),
		StaticMesh(InStaticMesh->StaticMesh)
	{

	}

    std::vector<Renderer::SStaticProxyData> GetStaticProxyData() const override
	{
		return
		{
			{
				{
					nullptr,
					StaticMesh->GetRenderData()->GetVertexBuffer(),
					StaticMesh->GetRenderData()->GetIndexBuffer(),
					StaticMesh->GetIndexCount(),
					StaticMesh->GetRenderData()->GetIndexFormat(),
					Renderer::EMeshRenderPass::BasePass
				}
			}
		};
	}
private:
	std::shared_ptr<CStaticMesh> StaticMesh;
};

TOwnerPtr<Renderer::CRenderableComponentProxy> SStaticMeshComponent::InstantiateProxy(
	Renderer::CWorldProxy* InWorld) const
{
	if (!StaticMesh || !StaticMesh->GetRenderData())
		return nullptr;

	return new CStaticMeshComponentProxy(InWorld, this);
}

} /** namespace Components */