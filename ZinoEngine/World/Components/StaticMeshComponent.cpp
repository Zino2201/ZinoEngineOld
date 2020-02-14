#include "StaticMeshComponent.h"
#include "Core/RenderThread.h"
#include "Render/Commands/RenderCommands.h"
#include "Render/StaticMesh.h"
#include "Render/Material/Material.h"
#include "Core/Engine.h"
#include "Render/Window.h"
#include "Render/ShaderAttributesManager.h"

/**
 * Render thread version of CStaticMeshComponent
 */
class CStaticMeshComponentProxy final : public CRenderableComponentProxy
{
public:
	CStaticMeshComponentProxy(const CStaticMeshComponent* InComponent) :
		CRenderableComponentProxy(InComponent), StaticMesh(InComponent->GetStaticMesh()),
		Material(InComponent->GetMaterial()),
		VertexIndexBuffer(InComponent->GetStaticMesh()->GetVertexIndexBuffer())
	{
		// TODO: reset transform
		if(Material)
			ShaderAttributesManager = Material->GetRenderData()->GetPipeline()->CreateShaderAttributesManager(
				EShaderAttributeFrequency::PerInstance);
	}

	virtual void Draw(CRenderCommandList* InCommandList) override
	{
		if(!VertexIndexBuffer || !Material || !ShaderAttributesManager)
			return;

		verify(IsInRenderThread());

		InCommandList->Enqueue<CRenderCommandBindGraphicsPipeline>(
			Material->GetRenderData()->GetPipeline());

		Material->SetVec3("Ambient", glm::vec3(.0215f, .1745f, .0215f));
		Material->SetVec3("Diffuse", glm::vec3(.07568f, .61424f, .07568f));
		Material->SetVec3("Specular", glm::vec3(.633f, .727811f, .633f));
		Material->SetFloat("Shininess", 76.8f);

		InCommandList->Enqueue<CRenderCommandBindShaderAttributeManager>(
			Material->GetRenderData()->GetShaderAttributesManager());

		UBO.World = glm::translate(glm::mat4(1.f),
			Transform.Position);
		UBO.View = glm::lookAt(g_Engine->CameraPos, g_Engine->CameraPos + g_Engine->CameraFront,
			g_Engine->CameraUp);
		UBO.Projection = glm::perspective(glm::radians(45.f),
			(float)g_Engine->GetWindow()->GetWidth() / 
			(float)g_Engine->GetWindow()->GetHeight(), 0.1f, 10000.0f);
		UBO.Projection[1][1] *= -1;
		UBO.CamPos = g_Engine->CameraPos;

		glm::vec3 LightPos = glm::vec3(0, 5, 10);

		ShaderAttributesManager->SetUniformBuffer("LightUBO",
			&LightPos);

		ShaderAttributesManager->SetUniformBuffer("UBO",
			&UBO);

		InCommandList->Enqueue<CRenderCommandBindShaderAttributeManager>(
			ShaderAttributesManager);
		InCommandList->Enqueue<CRenderCommandBindVertexBuffers>(
			VertexIndexBuffer->GetVertexBuffer());
		InCommandList->Enqueue<CRenderCommandBindIndexBuffer>(
			VertexIndexBuffer->GetIndexBuffer(), 0,
			VertexIndexBuffer->GetOptimalIndexFormat());
		InCommandList->Enqueue<CRenderCommandDrawIndexed>(
			VertexIndexBuffer->GetIndexCount(), 1, 0, 0, 0);
	}
private: 
	CStaticMeshVertexIndexBuffer* VertexIndexBuffer;
	std::shared_ptr<CStaticMesh> StaticMesh;
	std::shared_ptr<CMaterial> Material;
	STestUBO UBO;
};

CStaticMeshComponent::CStaticMeshComponent() : CRenderableComponent() {}

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