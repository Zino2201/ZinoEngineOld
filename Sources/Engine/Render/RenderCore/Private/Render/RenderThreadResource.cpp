#include "Render/RenderThreadResource.h"
#include "Render/RenderThread.h"

namespace ZE
{

void CRenderThreadResource::InitResource()
{
	if(IsInRenderThread())
	{
		bIsInitialized = true;
		InitResource_RenderThread();
	}
	else
	{
		EnqueueRenderCommand("CRenderThreadResource::InitResource",
			[this]()
			{
				bIsInitialized = true;
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
		bIsInitialized = false;
		DestroyResource_RenderThread();
	}
	else
	{
		EnqueueRenderCommand("CRenderThreadResource::DestroyResource",
			[this]()
		{
			bIsInitialized = false;
			DestroyResource_RenderThread();
		});

		/** Wait for command to be executed */
		FlushRenderThread();
	}
}

} /* namespace ZE */