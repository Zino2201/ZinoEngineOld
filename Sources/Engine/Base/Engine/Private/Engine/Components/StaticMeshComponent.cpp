#include "Engine/Components/StaticMeshComponent.h"
#include "Reflection/Builders.h"
#include "Engine/Assets/StaticMesh.h"
#include "Renderer/RenderableComponentType.h"
#include "Renderer/RendererTransientData.h"

namespace ZE::Components
{

/** Component */
void SStaticMeshComponent::SetStaticMesh(const std::shared_ptr<CStaticMesh>& InSM)
{
	StaticMesh = InSM;
	ECS::CECSManager::Get().GetSystem<CRenderableComponentSystem>()
		->AddRenderableComponentToUpdate(this);
}

/** Component type */
class CStaticMeshRenderableComponentType : public Renderer::IRenderableComponentType
{
public:
	void CopyPerFrameComponentState(const Renderer::CRenderableComponentProxy& InComponentProxy,
		Renderer::STransientProxyDataPerFrame& OutPerFrameData) override
	{

	}

	void CopyComponentStatePerView(const Renderer::CRenderableComponentProxy& InComponentProxy,
		Renderer::STransientProxyDataPerView& OutPerViewData) override
	{

	}
};

/**
 * Proxy for a static mesh component
 */
class CStaticMeshComponentProxy : public Renderer::CRenderableComponentProxy
{
public:
    CStaticMeshComponentProxy(const SStaticMeshComponent* InStaticMesh) : 
		Renderer::CRenderableComponentProxy(InStaticMesh, 
			Renderer::ERenderableComponentProxyType::Dynamic),
		StaticMesh(InStaticMesh->StaticMesh)
	{

	}

	Renderer::IRenderableComponentType* GetComponentType() const override
	{
		static CStaticMeshRenderableComponentType* CompType = 
			Renderer::RegisterRenderableComponentType<CStaticMeshRenderableComponentType>();
		return CompType;
	}

	std::vector<Renderer::SMesh> GetDynamicMeshes(const Renderer::SWorldView& InView) const override 
	{ 
		return 
		{
			{
				{
					StaticMesh->GetRenderData()->GetVertexBuffer(),
					StaticMesh->GetRenderData()->GetIndexBuffer(),
					StaticMesh->GetRenderData()->GetIndexFormat(),
					StaticMesh->GetIndexCount()
				}
			}
		}; 
	}
private:
	std::shared_ptr<CStaticMesh> StaticMesh;
};

TOwnerPtr<Renderer::CRenderableComponentProxy> SStaticMeshComponent::InstantiateProxy() const
{
	if (!StaticMesh || !StaticMesh->GetRenderData())
		return nullptr;

	return new CStaticMeshComponentProxy(this);
}

} /** namespace Components */