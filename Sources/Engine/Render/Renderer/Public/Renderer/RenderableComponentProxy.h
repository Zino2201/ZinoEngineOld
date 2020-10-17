#pragma once

#include "EngineCore.h"
#include "Mesh.h"
#include "Render/UniformBuffer.h"

namespace ze::renderer
{

class IRenderableComponent;
class IRenderableComponentType;
struct SWorldView;

enum class ERenderableComponentProxyType
{
	Static,
	Dynamic
};

/**
 * Abstract class 
 * Renderer version of a SRenderableComponent
 * This is created by IRenderableComponentType and stored in CWorldProxy
 * It contains static data about a renderable component like its transform, its material
 */
class RENDERER_API CRenderableComponentProxy
{
public:
	CRenderableComponentProxy(const IRenderableComponent* InComponent,
		const ERenderableComponentProxyType& InType);
	virtual ~CRenderableComponentProxy() = default;

	virtual IRenderableComponentType* GetComponentType() const = 0;
	virtual std::vector<SMesh> GetDynamicMeshes(const SWorldView& InView) const = 0;

	const ERenderableComponentProxyType& GetType() const { return Type; }
	const maths::Matrix4& GetStaticWorldMatrix() const { return StaticWorldMatrix; }
	const IRenderableComponent* GetComponent() const { return Component; }
private:
	/** Pointer to the game version */
	const IRenderableComponent* Component;

	/** Static transform/world matrix, used for hint Static */
	maths::Transform StaticTransform;
	maths::Matrix4 StaticWorldMatrix;

	ERenderableComponentProxyType Type;
public:
	TUniformBuffer<glm::mat4> TEST_PerInstanceUBO;

};

}