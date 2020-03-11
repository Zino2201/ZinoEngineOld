#include "StaticMeshComponent.h"
#include "Core/RenderThread.h"
#include "Render/StaticMesh.h"
#include "Render/Material/Material.h"
#include "Core/Engine.h"
#include "Render/Window.h"
#include "Render/World/RenderableComponentProxy.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/Texture2D.h"
#include "Render/Commands/RenderCommandContext.h"

/**
 * Render thread version of CStaticMeshComponent
 */
class CStaticMeshComponentProxy final : public CRenderableComponentProxy
{
public:
	CStaticMeshComponentProxy(const CStaticMeshComponent* InComponent) :
		CRenderableComponentProxy(InComponent), StaticMesh(InComponent->GetStaticMesh().get()),
		Material(InComponent->GetMaterial().get()),
		VertexIndexBuffer(InComponent->GetStaticMesh()->GetVertexIndexBuffer())
	{
		LightUBO = g_Engine->GetRenderSystem()->CreateUniformBuffer(
			SRenderSystemUniformBufferInfos(12, true));
	}

	~CStaticMeshComponentProxy()
	{
		LightUBO->Destroy();
	}

	virtual IRenderSystemUniformBuffer* GetLightUBO() const override
	{
		return LightUBO.get();
	}

	virtual std::vector<SProxyStaticMeshData> GetStaticMeshDatas() override
	{
		std::vector<SProxyStaticMeshData> Datas =
		{ 
			SProxyStaticMeshData(Material->GetRenderData(),
				VertexIndexBuffer->GetVertexBuffer(), 
				VertexIndexBuffer->GetIndexBuffer(),
				VertexIndexBuffer->GetIndexCount(),
				VertexIndexBuffer->GetOptimalIndexFormat(),
				EMeshPass::GeometryPass)
		};

		return Datas;
	}
private: 
	CStaticMeshVertexIndexBuffer* VertexIndexBuffer;
	CStaticMesh* StaticMesh;
	CMaterial* Material;
	STestUBO UBO;
	IRenderSystemUniformBufferPtr LightUBO;
};

CStaticMeshComponent::CStaticMeshComponent() : CRenderableComponent() {}
CStaticMeshComponent::~CStaticMeshComponent() {}

void CStaticMeshComponent::SetStaticMesh(const std::shared_ptr<CStaticMesh>& InStaticMesh)
{
	StaticMesh = InStaticMesh;

	NeedRenderProxyUpdate();
}

CRenderableComponentProxy* CStaticMeshComponent::InstantiateRenderProxy() const
{
	if(!StaticMesh)
		return nullptr;

	return new CStaticMeshComponentProxy(this);
}