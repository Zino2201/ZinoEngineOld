#include "Render/RenderThreadResource.h"
#include "Render/RenderThread.h"

namespace ZE
{

void CRenderThreadResource::InitResource()
{
	if(IsInRenderThread())
	{
		InitResource_RenderThread();
	}
	else
	{
		EnqueueRenderCommand("CRenderThreadResource::InitResource",
			[this]()
			{
				InitResource_RenderThread();
			});

		/** Wait for command to be executed */
		FlushRenderThread();
	}
}

void CRenderThreadResource::DestroyResource()
{
	if (IsInRenderThread())
	{
		DestroyResource_RenderThread();
	}
	else
	{
		EnqueueRenderCommand("CRenderThreadResource::DestroyResource",
			[this]()
		{
			DestroyResource_RenderThread();
		});

		/** Wait for command to be executed */
		FlushRenderThread();
	}
}

} /* namespace ZE */