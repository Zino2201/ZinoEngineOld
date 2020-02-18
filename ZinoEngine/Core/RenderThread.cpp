#include "RenderThread.h"
#include "World/Components/RenderableComponent.h"
#include "Core/Engine.h"
#include "Render/Renderer/ForwardSceneRenderer.h"
#include "World/World.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/Commands/Commands.h"
#include "Render/Commands/RenderCommandContext.h"

CRenderResource::~CRenderResource() { must(!bInitialized); }

void CRenderResource::InitResources()
{
	if(IsInRenderThread())
	{
		InitRenderThread();
		bInitialized = true;
	}
	else
	{
		EnqueueRenderCommand(
			[this](CRenderCommandList* InCommandList)
		{
			InitRenderThread();
			bInitialized = true;
		});
	}
}

void CRenderResource::DestroyResources()
{
	if (IsInRenderThread())
	{
		DestroyRenderThread();
		bInitialized = false;
		DestroyedSemaphore.Notify();
	}
	else
	{
		EnqueueRenderCommand(
			[this](CRenderCommandList* InCommandList)
		{
			DestroyRenderThread();
			bInitialized = false;
			DestroyedSemaphore.Notify();
		});
	}
}

/** Render Thread */

CRenderThread::CRenderThread() {}
CRenderThread::~CRenderThread() {}

void CRenderThread::Initialize()
{
	LoopRenderThread = true;
	RenderThreadCommandList = new CRenderCommandList;
}

void CRenderThread::Start()
{
	if(!RenderThreadHandle.joinable())
		RenderThreadHandle = std::thread(&CRenderThread::RenderThreadMain, this);
}

void CRenderThread::RenderThreadMain()
{
	RenderThreadCommandList->Initialize();
	RenderThreadID = std::this_thread::get_id();
	ShouldRender = true;

	std::unique_ptr<CForwardSceneRenderer> ForwardRenderer =
		std::make_unique<CForwardSceneRenderer>();

	while (LoopRenderThread)
	{
		/** Execute all commands coming from game thread */
		while (RenderThreadCommandList->GetCommandsCount() > 0)
		{
			RenderThreadCommandList->ExecuteFrontCommand();
		}

		/** Render world, skip first frame */
		if(ShouldRender)
		{
			g_Engine->GetRenderSystem()->Prepare();
			RenderThreadCommandList->GetCommandContext()->Begin();
			ForwardRenderer->Render(RenderThreadCommandList->GetCommandContext(),
				g_Engine->GetWorld()->GetScene());
			RenderThreadCommandList->GetCommandContext()->End();
			g_Engine->GetRenderSystem()->Present();
		}

		g_Engine->RenderThreadCounter++;

		/** Notify the game thread that we have finished */
		RenderThreadSemaphore.Notify();
	}

	LOG(ELogSeverity::Debug, "Render thread exiting loop")
}

void CRenderThread::Stop()
{
	LOG(ELogSeverity::Debug, "Stopping render thread")
	LoopRenderThread = false;
	RenderThreadHandle.join();
	delete RenderThreadCommandList;
}