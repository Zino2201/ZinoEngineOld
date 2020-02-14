#include "RenderThread.h"
#include "World/Components/RenderableComponent.h"
#include "Core/Engine.h"
#include "Render/Commands/RenderCommands.h"
#include "Render/Renderer/ForwardSceneRenderer.h"
#include "World/World.h"
#include "Render/RenderSystem/RenderSystem.h"

CRenderResource::~CRenderResource() { must(!bInitialized) }

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
	}
	else
	{
		EnqueueRenderCommand(
			[this](CRenderCommandList* InCommandList)
		{
			DestroyRenderThread();
			bInitialized = false;
		});
	}
}

CRenderableComponentProxy::CRenderableComponentProxy(const CRenderableComponent* InComponent) :
	Transform(InComponent->GetTransform()) {}

void CRenderableComponentProxy::Draw(CRenderCommandList* InCommandList) {}

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
		/** Wait the game thread if we are too fast */

		/** Render world */

		if(ShouldRender)
		{
			g_Engine->GetRenderSystem()->Prepare();
			RenderThreadCommandList->Enqueue<CRenderCommandBeginRecording>();
			ForwardRenderer->Render(RenderThreadCommandList,
				g_Engine->GetWorld()->GetScene());
			RenderThreadCommandList->Enqueue<CRenderCommandEndRecording>();
		}

		/** Execute all commands */
		while (RenderThreadCommandList->GetCommandsCount() > 0)
		{
			RenderThreadCommandList->ExecuteFrontCommand();
		}

		if(ShouldRender)
			g_Engine->GetRenderSystem()->Present();

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