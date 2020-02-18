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
		// TODO: reset transform

		UniformBuffer = g_Engine->GetRenderSystem()->CreateUniformBuffer(
			SRenderSystemUniformBufferInfos(192, true));

		LightUBO = g_Engine->GetRenderSystem()->CreateUniformBuffer(
			SRenderSystemUniformBufferInfos(12, true));
	}

	~CStaticMeshComponentProxy()
	{
		UniformBuffer->Destroy();
		LightUBO->Destroy();
	}

	virtual void Draw(IRenderCommandContext* InCommandContext) override
	{
		if(!VertexIndexBuffer || !Material)
			return;

		verify(IsInRenderThread());

		InCommandContext->BindGraphicsPipeline(Material->GetRenderData()->GetPipeline());

		InCommandContext->SetShaderCombinedImageSampler(0, 0,
			Material->TestTexture->GetResource()->GetTextureView());

		InCommandContext->SetShaderUniformBuffer(0, 1,
			Material->GetRenderData()->GetUniformBuffer());

		UBO.World = glm::translate(glm::mat4(1.f),
			Transform.Position);
		UBO.View = glm::lookAt(g_Engine->CameraPos, g_Engine->CameraPos + g_Engine->CameraFront,
			g_Engine->CameraUp);
		UBO.Projection = glm::perspective(glm::radians(45.f),
			(float)g_Engine->GetWindow()->GetWidth() / 
			(float)g_Engine->GetWindow()->GetHeight(), 0.1f, 10000.0f);
		UBO.Projection[1][1] *= -1;
		UBO.CamPos = g_Engine->CameraPos;
		memcpy(UniformBuffer->GetMappedMemory(), &UBO, sizeof(UBO));

		InCommandContext->SetShaderUniformBuffer(1, 0,
			UniformBuffer.get());

		glm::vec3 LightPos = glm::vec3(0, 5, 10);
		memcpy(LightUBO->GetMappedMemory(), &LightPos, sizeof(LightPos));

		InCommandContext->SetShaderUniformBuffer(1, 1,
			LightUBO.get());

		InCommandContext->BindVertexBuffers({ VertexIndexBuffer->GetVertexBuffer() });
		InCommandContext->BindIndexBuffer(VertexIndexBuffer->GetIndexBuffer(),
			0,
			VertexIndexBuffer->GetOptimalIndexFormat());
		InCommandContext->DrawIndexed(VertexIndexBuffer->GetIndexCount(), 1, 0, 0, 0);
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