#include "RenderThread.h"
#include "World/Components/RenderableComponent.h"
#include "Core/Engine.h"
#include "Render/Renderer/ForwardSceneRenderer.h"
#include "World/World.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/Commands/Commands.h"
#include "Render/Commands/RenderCommandContext.h"
#include "UI/imgui.h"
#include "UI/imgui_impl_sdl.h"
#include "Render/Window.h"
#include "Render/Renderer/Scene.h"

DECLARE_TIMER_STAT(Rendering, RenderThreadFrameTime);
DECLARE_TIMER_STAT(Rendering, UpdateStatsCurves);

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
		g_Engine->GetRenderSystem()->WaitGPU();
		DestroyRenderThread();
		bInitialized = false;
		DestroyedSemaphore.Notify();
	}
	else
	{
		EnqueueRenderCommand(
			[this](CRenderCommandList* InCommandList)
		{
			g_Engine->GetRenderSystem()->WaitGPU();
			DestroyRenderThread();
			bInitialized = false;
			DestroyedSemaphore.Notify();
		});
	}
}

/** Frame destruction resource */

std::queue<IDeferredDestructionRenderResource*> IDeferredDestructionRenderResource::Resources;

void IDeferredDestructionRenderResource::Destroy()
{
	Resources.push(this);
}

void IDeferredDestructionRenderResource::DestroyResources()
{
	while(!Resources.empty())
	{
		IDeferredDestructionRenderResource* Resource = Resources.front();
		Resource->FinishDestroy();
		delete Resource;	
		Resources.pop();
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

DECLARE_TIMER_STAT_EXTERN(GameThreadFrameTime);
DECLARE_COUNTER_STAT_EXTERN(Drawcall);

void DisplayStats(const EStatGroupCategory& InCategory)
{
	for (const auto& Group : *CStatManager::StatGroupSet)
	{
		if (Group->GetCategory() == InCategory)
		{
			ImGui::Text("-- %s", Group->GetName().c_str());
			ImGui::Columns(2, Group->GetName().c_str());
			ImGui::Separator();
			ImGui::Text("ID");
			ImGui::NextColumn();
			ImGui::Text("Value (last frame)");
			ImGui::NextColumn();
			ImGui::Separator();

			for (const auto& Stat : Group->GetStats())
			{
				//ImGui::NextColumn();
				ImGui::Text("%s", Stat->GetID().c_str());
				ImGui::NextColumn();
				switch (Stat->GetDataType())
				{
				case EStatDataType::Float:
				{
					float* FltPtr = reinterpret_cast<float*>(Stat->GetData());
					ImGui::Text("%f ms", *FltPtr);
					break;
				}
				case EStatDataType::Uint32:
				{
					uint32_t* UintPtr = reinterpret_cast<uint32_t*>(Stat->GetData());
					ImGui::Text("%d", *UintPtr);
					break;
				}
				}
				ImGui::NextColumn();
			}

			ImGui::Columns(1);
			ImGui::Separator();
		}
	}
}

void CRenderThread::RenderThreadMain()
{
	RenderThreadCommandList->Initialize();
	RenderThreadID = std::this_thread::get_id();
	ShouldRender = true;

	g_Engine->GetRenderSystem()->GetFrameCompletedDelegate().Bind(
		std::bind(&IDeferredDestructionRenderResource::DestroyResources));

	SceneRenderer = new CForwardSceneRenderer;

	Uint64 Now = SDL_GetPerformanceCounter();
	Uint64 Last = 0;
	float DeltaTime;

	std::vector<float> FrameTimes;
	std::vector<float> GTFrameTimes;
	std::vector<float> FPSArray;

	int ClearAtCountValue = 100;
	float FPS;

	while (LoopRenderThread)
	{
		SCOPED_TIMER_STAT(RenderThreadFrameTime);
		{
			SCOPED_TIMER_STAT(UpdateStatsCurves);

			if (FrameTimes.size() > ClearAtCountValue)
			{
				for (size_t i = 1; i < FrameTimes.size(); ++i)
				{
					FrameTimes[i - 1] = FrameTimes[i];
				}

				FrameTimes[FrameTimes.size() - 1] = StatRenderThreadFrameTime.GetDataTemplate();
			}
			else
			{
				FrameTimes.push_back(StatRenderThreadFrameTime.GetDataTemplate());
			}

			if (GTFrameTimes.size() > ClearAtCountValue)
			{
				for (size_t i = 1; i < GTFrameTimes.size(); ++i)
				{
					GTFrameTimes[i - 1] = GTFrameTimes[i];
				}

				GTFrameTimes[GTFrameTimes.size() - 1] = StatGameThreadFrameTime.GetDataTemplate();
			}
			else
			{
				GTFrameTimes.push_back(StatGameThreadFrameTime.GetDataTemplate());
			}

			if (FPSArray.size() > ClearAtCountValue)
			{
				for (size_t i = 1; i < FPSArray.size(); ++i)
				{
					FPSArray[i - 1] = FPSArray[i];
				}

				FPSArray[FPSArray.size() - 1] = FPS;
			}
			else
			{
				FPSArray.push_back(FPS);
			}
		}

		Last = Now;
		Now = SDL_GetPerformanceCounter();

		DeltaTime = (((Now - Last) * 1000 / (float)SDL_GetPerformanceFrequency()))
			* 0.001f;

		FPS = 1.0f / DeltaTime;

		/** Execute all commands coming from game thread */
		while (RenderThreadCommandList->GetCommandsCount() > 0)
		{
			RenderThreadCommandList->ExecuteFrontCommand();
		}

		/** Render world, skip first frame */
		if(ShouldRender && SceneRenderer)
		{
			ImGui_ImplSDL2_NewFrame(g_Engine->GetWindow()->GetSDLWindow());
			ImGui::NewFrame();
			
			ImGui::BeginMainMenuBar();
			ImGui::Text(
				"FPS: %i | X: %f Y: %f Z: %f | RT/GT frame time: %.2f/%.2f ms | Drawcalls: %d | Actors: %d | Renderable proxies: %d | Render System: %s", 
				static_cast<int>(FPS),
				g_Engine->CameraPos.x,
				g_Engine->CameraPos.y,
				g_Engine->CameraPos.z,
				StatRenderThreadFrameTime.GetDataTemplate(),
				StatGameThreadFrameTime.GetDataTemplate(),
				StatDrawcall.GetDataTemplate(),
				g_Engine->GetWorld()->GetActors().size(),
				g_Engine->GetWorld()->GetScene()->GetProxies().size(),
				g_Engine->GetRenderSystem()->GetRenderSystemDetails().Name.c_str());
			ImGui::EndMainMenuBar();

			float GTAverage = 0.0f;
			float RTAverage = 0.0f;

			ImGui::Begin("Stat Window");
			if(!FrameTimes.empty())
			{
				for (int i = 0; i < FrameTimes.size(); ++i)
					RTAverage += FrameTimes[i];
				RTAverage /= (float) FrameTimes.size();
				char overlay[32];
				sprintf_s(overlay, "Average: %f ms", RTAverage);
				ImGui::PlotLines("RT frame times", FrameTimes.data(), FrameTimes.size(),
					0, overlay);
			}

			if(!GTFrameTimes.empty())
			{
				for (int i = 0; i < GTFrameTimes.size(); ++i)
					GTAverage += GTFrameTimes[i];
				GTAverage /= (float)GTFrameTimes.size();
				char overlay[32];
				sprintf_s(overlay, "Average: %f ms", GTAverage);
				ImGui::PlotLines("GT frame times", GTFrameTimes.data(), GTFrameTimes.size(),
					0, overlay);
			}

			if (!FPSArray.empty())
			{
				float Average = 0.0f;
				for (int i = 0; i < FPSArray.size(); ++i)
					Average += FPSArray[i];
				Average /= (float)FPSArray.size();
				char overlay[64];
				sprintf_s(overlay, "Average: %f fps", Average);
				ImGui::PlotLines("FPS (RT)", FPSArray.data(), FPSArray.size(),
					0, overlay);
			}

			ImGui::Text("GT/RT: %f/%f ms", GTAverage, RTAverage);
			if(GTAverage > RTAverage)
				ImGui::TextColored(ImVec4(1,0,0,1), "GT is slower");
			else
				ImGui::TextColored(ImVec4(1,0,0,1), "RT is slower");

			ImGui::Text("-- RENDERING STATS --");
			DisplayStats(EStatGroupCategory::Rendering);
			ImGui::Text("-- SIMULATION STATS --");
			DisplayStats(EStatGroupCategory::Simulation);

			ImGui::End();

			CStatManager::ResetStats(EStatGroupCategory::Rendering);

			ImGui::Render();
			
			/** VIEWPORT */
			SViewport Viewport;
			Viewport.Rect.Position = { 0, 0 };
			Viewport.Rect.Size = { g_Engine->GetWindow()->GetWidth(),
				g_Engine->GetWindow()->GetHeight() };
			Viewport.MinDepth = 0.f;
			Viewport.MaxDepth = 1.f;

			g_Engine->GetRenderSystem()->Prepare();
			SceneRenderer->PreRender();
			RenderThreadCommandList->GetCommandContext()->Begin();
			SceneRenderer->Render(RenderThreadCommandList->GetCommandContext(),
				g_Engine->GetWorld()->GetScene(),
				Viewport);
			RenderThreadCommandList->GetCommandContext()->End();
			g_Engine->GetRenderSystem()->Present();
		}

		g_Engine->RenderThreadCounter++;

		//if(ShouldWaitGT)
		//	g_Engine->GameThreadSemaphore.Wait();

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

void CRenderThread::DestroyRenderer()
{
	delete SceneRenderer;
}