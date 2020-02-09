#include "StaticMeshComponent.h"
#include "Core/RenderThread.h"
#include "Render/Commands/RenderCommands.h"
#include "Render/StaticMesh.h"

class CStaticMeshComponentProxy : public CRenderableComponentProxy
{
public:
	CStaticMeshComponentProxy(const CStaticMeshComponent* InComponent) :
		CRenderableComponentProxy(InComponent), StaticMesh(InComponent->GetStaticMesh()) {}

	virtual void Draw(CRenderCommandList* InCommandList) override
	{
		InCommandList->Enqueue<CRenderCommandBindVertexBuffers>(
			StaticMesh->GetVertexBuffer());
		InCommandList->Enqueue<CRenderCommandBindIndexBuffer>(
			StaticMesh->GetIndexBuffer(), 0,
			StaticMesh->GetOptimalIndexFormat());
		InCommandList->Enqueue<CRenderCommandDrawIndexed>(
			StaticMesh->GetIndexCount(), 1, 0, 0, 0);
	}
private:
	std::shared_ptr<CStaticMesh> StaticMesh;
};

void CStaticMeshComponent::SetStaticMesh(const std::shared_ptr<CStaticMesh>& InStaticMesh)
{
	StaticMesh = InStaticMesh;
}

CRenderableComponentProxy* CStaticMeshComponent::CreateRenderProxy() const
{
	return new CStaticMeshComponentProxy(this);
}