#include "Renderer/RenderableComponentType.h"

namespace ZE::Renderer
{

static std::vector<std::unique_ptr<IRenderableComponentType>> ComponentTypes;

IRenderableComponentType* RegisterRenderableComponentType(
	TOwnerPtr<IRenderableComponentType> InComponentType)
{
	ComponentTypes.emplace_back(InComponentType);

	return InComponentType;
}

}