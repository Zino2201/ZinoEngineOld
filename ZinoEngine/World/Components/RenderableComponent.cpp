#include "RenderableComponent.h"
#include "Render/ShaderAttributesManager.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/Material/Material.h"
#include "Core/Engine.h"
#include "Core/RenderThread.h"

CRenderableComponent::CRenderableComponent() 
{
	/** Create render proxy */
	RenderProxy.reset(CreateRenderProxy());
	
	/** Add it to the scene */
	CEngine::Get().GetSceneProxy()->AddRenderableComponentProxy(RenderProxy.get());
}

CRenderableComponent::~CRenderableComponent() {}

void CRenderableComponent::SetMaterial(const std::shared_ptr<CMaterial>& InMaterial)
{
	Material = InMaterial;

	ShaderAttributesManager = Material->GetPipeline()->CreateShaderAttributesManager(
		EShaderAttributeFrequency::PerInstance);
}

CRenderableComponentProxy* CRenderableComponent::CreateRenderProxy() const
{
	return new CRenderableComponentProxy(this);
}
