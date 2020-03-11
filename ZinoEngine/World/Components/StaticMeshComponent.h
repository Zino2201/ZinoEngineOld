#pragma once

#include "RenderableComponent.h"

class CStaticMesh;

struct STestUBO
{
	alignas(16) glm::mat4 World;
	alignas(16) glm::mat4 View;
	alignas(16) glm::mat4 Projection;
	alignas(16) glm::vec3 CamPos;
	alignas(16) glm::vec3 ViewDir;
};

/**
 * Component that render a mesh
 */
ZCLASS()
class CStaticMeshComponent : public CRenderableComponent
{
	REFLECTED_CLASS(CWorldComponent)

public:
	CStaticMeshComponent();
	~CStaticMeshComponent(); 

	void SetStaticMesh(const std::shared_ptr<CStaticMesh>& InStaticMesh);

	virtual CRenderableComponentProxy* InstantiateRenderProxy() const override;

	const std::shared_ptr<CStaticMesh>& GetStaticMesh() const { return StaticMesh; }
public:
	STestUBO UBO;
private:
	std::shared_ptr<CStaticMesh> StaticMesh;
};