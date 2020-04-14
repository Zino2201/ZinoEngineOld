#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Module/Module.h"

namespace ZE
{

DEFINE_MODULE(CDefaultModule, "RenderSystem")

RENDERSYSTEM_API extern IRenderSystem* GRenderSystem = nullptr;
RENDERSYSTEM_API extern IRenderSystemContext* GRSContext = nullptr;
//
//TMulticastDelegate<IRenderSystemResource*> OnResourceDestroyed;
//
//void IRenderSystemResource::Destroy()
//{
//	OnResourceDestroyed.Broadcast(this);
//}

} /** namespace ZE */