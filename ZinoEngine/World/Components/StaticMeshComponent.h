#pragma once

#include "RenderableComponent.h"

class CStaticMesh;

struct STestUBO
{
	alignas(16) glm::mat4 World;
	alignas(16) glm::mat4 View;
	alignas(16) glm::mat4 Projection;
	alignas(16) glm::vec3 CamPos;
};

/**
 * Component that render a mesh
 */
class CStaticMeshComponent : public CRenderableComponent
{
	REFLECTED_CLASS(CWorldComponent)

public:
	void SetStaticMesh(const std::shared_ptr<CStaticMesh>& InStaticMesh);

	virtual CRenderableComponentProxy* CreateRenderProxy() const override;

	const std::shared_ptr<CStaticMesh>& GetStaticMesh() const { return StaticMesh; }
public:
	STestUBO UBO;
private:
	std::shared_ptr<CStaticMesh> StaticMesh;
};