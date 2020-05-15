#include "Renderer/RendererModule.h"
#include "Renderer/FrameGraph/RenderPass.h"

DEFINE_MODULE(ZE::Renderer::CRendererModule, "Renderer");

namespace ZE::Renderer
{

void CRendererModule::Destroy()
{
	CRenderPassPersistentResourceManager::Get().Destroy();
}

}