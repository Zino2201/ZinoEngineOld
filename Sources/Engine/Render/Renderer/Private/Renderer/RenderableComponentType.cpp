#include "Renderer/RenderableComponentType.h"

namespace ze::renderer
{

static std::vector<std::unique_ptr<IRenderableComponentType>> ComponentTypes;

IRenderableComponentType* RegisterRenderableComponentType(
	OwnerPtr<IRenderableComponentType> InComponentType)
{
	ComponentTypes.emplace_back(InComponentType);

	return InComponentType;
}

}