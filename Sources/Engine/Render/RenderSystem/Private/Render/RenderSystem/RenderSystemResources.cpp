#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Module/Module.h"
#include "Render/RenderSystem/Resources/Buffer.h"

namespace ZE
{

DEFINE_MODULE(ZE::Module::CDefaultModule, RenderSystem)

RENDERSYSTEM_API extern IRenderSystem* GRenderSystem = nullptr;
RENDERSYSTEM_API extern IRenderSystemContext* GRSContext = nullptr;

namespace RSUtils
{

void Copy(const void* Src, CRSBuffer* Dst)
{
	void* Data = Dst->Map(ERSBufferMapMode::WriteOnly);
	memcpy(Data, Src, Dst->GetCreateInfo().Size);
	Dst->Unmap();
}

}


} /** namespace ZE */