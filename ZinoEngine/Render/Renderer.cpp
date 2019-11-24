#include "Renderer.h"
#include "Commands/Commands.h"

CRenderer::CRenderer()
	: MainCommandList(std::make_unique<CRenderCommandList>())
{

}