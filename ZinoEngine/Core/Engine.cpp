#include "Engine.h"
#include "Render/Window.h"
#include <SDL2/SDL.h>
#include "Render/RenderSystem/Vulkan/VulkanRenderSystem.h"
#include "IO/IOUtils.h"
#include "Render/Renderer/ForwardSceneRenderer.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "AssetManager.h"
#include "Render/Material/Material.h"
#include <stb_image.h>
#include "Render/Texture2D.h"
#include "Render/StaticMesh.h"
#include "World/World.h"
#include "World/StaticMeshActor.h"
#include "World/Components/StaticMeshComponent.h"
#include "Render/Shader.h"
#include <glslang/Public/ShaderLang.h>
#include "Render/Shaders/BasicShader.h"
#include "Render/Commands/Commands.h"
#include "Core/RenderThread.h"
#include "Core/Stats/StatThread.h"
#include "UI/imgui.h"
#include "UI/imgui_impl_sdl.h"
#include "Render/Renderer/Mesh/MeshPass.h"

CEngine* g_Engine = nullptr;
DECLARE_TIMER_STAT(GameThread, GameThreadFrameTime);

std::thread::id GameThreadID;
std::thread::id RenderThreadID;

CEngine::CEngine() : bHasBeenInitialized(false) {}
CEngine::~CEngine() {}

void CEngine::Initialize()
{
	GameThreadID = std::this_thread::get_id();

	if (bHasBeenInitialized)
	{
		LOG(ELogSeverity::Error, "Engine already initialized")
		return;
	}
	
	bHasBeenInitialized = true;

	glslang::InitializeProcess();

	Window = std::make_unique<CWindow>(1280, 720, "ZinoEngine");

	/** Initialize render command list early */
	CRenderThread::Get().Initialize();

	/** Initialize ImGui */
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& IO = ImGui::GetIO();
	IO.DisplaySize = ImVec2(Window->GetWidth(), Window->GetHeight());
	IO.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

	RenderSystem = std::make_unique<CVulkanRenderSystem>();
	RenderSystem->Initialize();
	LOG(ELogSeverity::Info, "Using render system %s", 
		RenderSystem->GetRenderSystemDetails().Name.c_str());
	/** Create asset manager after render system, 
	 * so we can free all assets before destroying render system */
	AssetManager = std::make_unique<CAssetManager>();
	CBasicShaderClass::InstantiateBasicShaders();
	World = std::make_unique<CWorld>();
	CRenderableComponent::InitStatics();

#ifdef DEVELOPMENT
	CStatThread::Start();
#endif

	Loop();
}

struct SMaterialTest
{
	alignas(16) glm::vec3 Ambient;
	alignas(16) glm::vec3 Diffuse;
	alignas(16) glm::vec3 Specular;
	float Shininess;
};

void CEngine::Loop()
{
	LOG(ELogSeverity::Info, "Shader classes: %d", CShaderClass::GetShaderClassMap().size());

	/** Material test */
	std::shared_ptr<CMaterial> Material = AssetManager->Get<CMaterial>("Materials/test.json");

	SMaterialTest MatTest;
	//Material->SetVec3("Ambient", glm::vec3(.0215f, .1745f, .0215f));
//Material->SetVec3("Diffuse", glm::vec3(.07568f, .61424f, .07568f));
//Material->SetVec3("Specular", glm::vec3(.633f, .727811f, .633f));
//Material->SetFloat("Shininess", 76.8f);
	MatTest.Ambient = glm::vec3(.0215f, .1745f, .0215f);
	MatTest.Diffuse = glm::vec3(.07568f, .61424f, .07568f);
	MatTest.Specular = glm::vec3(.633f, .727811f, .633f);
	MatTest.Shininess = 76.8f;

	//Material->SetMaterialUBO(&MatTest, sizeof(MatTest));

	std::vector<std::shared_ptr<CStaticMesh>> Meshes =
	{
		AssetManager->Get<CStaticMesh>("MergerSponge.obj"),
	};

	for(int i = 0; i < 500; ++i)
	{
		std::shared_ptr<CStaticMeshActor> Actor = World->SpawnActor<CStaticMeshActor>(
			STransform(glm::dvec3(rand() % 100, rand() % 100, rand() % 100)));
		Actor->GetStaticMesh()->SetStaticMesh(Meshes[(rand() % Meshes.size())]);
		Actor->GetStaticMesh()->SetMaterial(Material);
	}

	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 LightPos = glm::vec3(0, 5, 10);
	CameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 CameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 CameraDirection = glm::normalize(CameraPos - CameraTarget);
	glm::vec3 CameraRight = glm::normalize(glm::cross(Up, CameraDirection));
	CameraUp = glm::cross(CameraDirection, CameraRight);
	CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

	std::array<float, 4> ClearColor = { 0.f, 0.f, 0.f, 1.0f };

	float CamYaw = 0.f, CamPitch = 0.f;
	bool bIsMouseGrabbed = false;

	SDL_WarpMouseInWindow(Window->GetSDLWindow(), Window->GetWidth() / 2, Window->GetHeight() / 2);
	SDL_SetRelativeMouseMode(static_cast<SDL_bool>(bIsMouseGrabbed));

	GameLoop = true;

	Uint64 Now = SDL_GetPerformanceCounter();
	Uint64 Last = 0;

	/** Main thread = game thread */
	while (GameLoop)
	{
		SCOPED_TIMER_STAT(GameThreadFrameTime);

		/** Delta time */
		Last = Now;
		Now = SDL_GetPerformanceCounter();

		DeltaTime = (((Now - Last) * 1000 / (float) SDL_GetPerformanceFrequency()))
			* 0.001f;

		/** Used for materials */
		ElapsedTime += DeltaTime;

		float Sensitivity = 0.25f;

		/** Event handling */

		float CameraSpeed = 20.f * DeltaTime;

		ImGuiIO& IO = ImGui::GetIO();

		SDL_Event Event;
		while (SDL_PollEvent(&Event))
		{
			ImGui_ImplSDL2_ProcessEvent(&Event);

			if (Event.type == SDL_WINDOWEVENT)
			{
				if (Event.window.event == SDL_WINDOWEVENT_MINIMIZED)
				{
					Uint32 Flags = SDL_GetWindowFlags(Window->GetSDLWindow());
					while (Flags & SDL_WINDOW_MINIMIZED)
					{
						Flags = SDL_GetWindowFlags(Window->GetSDLWindow());
						SDL_WaitEvent(nullptr);
					}
				}
				else if (Event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					int NewWidth = 0;
					int NewHeight = 0;
					SDL_GetWindowSize(Window->GetSDLWindow(), &NewWidth, &NewHeight);
					Window->SetWidth(NewWidth);
					Window->SetHeight(NewHeight);
					Window->OnWindowResized.Broadcast();
				}
			} 
			if(Event.type == SDL_QUIT)
			{
				/** Stop rendering and ticking */
				Exit();
			}
			
			if(Event.type == SDL_MOUSEMOTION)
			{
				if(bIsMouseGrabbed)
				{
					float DeltaX = Event.motion.xrel * Sensitivity;
					float DeltaY = Event.motion.yrel * Sensitivity;

					CamYaw += DeltaX;
					CamPitch += -DeltaY;

					if (CamPitch > 89.0f)
						CamPitch = 89.0f;
					if (CamPitch < -89.0f)
						CamPitch = -89.0f;

					glm::vec3 Front;
					Front.x = cos(glm::radians(CamYaw)) * cos(glm::radians(CamPitch));
					Front.y = sin(glm::radians(CamPitch));
					Front.z = sin(glm::radians(CamYaw)) * cos(glm::radians(CamPitch));
					CameraFront = glm::normalize(Front);

					SDL_WarpMouseInWindow(Window->GetSDLWindow(), Window->GetWidth() / 2, Window->GetHeight() / 2);
				}
			}
		}

		const Uint8* KeyState = SDL_GetKeyboardState(nullptr);
		Uint32 MouseState = SDL_GetMouseState(nullptr, nullptr);

		if(MouseState & SDL_BUTTON_LMASK 
			&& !bIsMouseGrabbed
			&& !ImGui::IsAnyItemHovered()
			&& !ImGui::IsAnyWindowHovered()
			&& !ImGui::IsAnyItemActive()
			&& !ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			SDL_SetRelativeMouseMode(SDL_TRUE);
			bIsMouseGrabbed = true;
		}
		if(KeyState[SDL_SCANCODE_W])
		{
			CameraPos += CameraSpeed * CameraFront;
		}
		if (KeyState[SDL_SCANCODE_S])
		{
			CameraPos -= CameraSpeed * CameraFront;
		}
		if (KeyState[SDL_SCANCODE_A])
		{
			CameraPos -= glm::normalize(glm::cross(CameraFront, CameraUp)) * CameraSpeed;
		}
		if (KeyState[SDL_SCANCODE_D])
		{
			CameraPos += glm::normalize(glm::cross(CameraFront, CameraUp)) * CameraSpeed;
		}
		if (KeyState[SDL_SCANCODE_ESCAPE])
		{
			if (bIsMouseGrabbed)
			{
				SDL_SetRelativeMouseMode(SDL_FALSE);
				bIsMouseGrabbed = false;
			}
		}

		TimerManager.UpdateTimers(DeltaTime);

		World->Tick(DeltaTime);

		/** Start render thread if not started */
		CRenderThread::Get().Start();

		/** Wait render thread before updating proxies and other 
		 * if it's not waiting us */
		CRenderThread::Get().GetRenderThreadSemaphore().Wait();

		/** Update render proxies */
		CRenderableComponent::UpdateRenderProxies();

		/** Notify render thread we have finished */
		GameThreadSemaphore.Notify();

		g_Engine->GameThreadCounter++;
	}
}

void CEngine::Exit()
{
	LOG(ELogSeverity::Info, "Exiting")
	GameLoop = false;

	/** Stop rendering */
	CRenderThread::Get().ShouldRender = false;
	CRenderThread::Get().ShouldWaitGT = false;

	/** Destroy renderer */
	EnqueueRenderCommand([](CRenderCommandList* InCmdList){ CRenderThread::Get().DestroyRenderer(); });

	/** Wait for render thread to finish rendering before destroying world */
	CRenderThread::Get().GetRenderThreadSemaphore().Wait();
	RenderSystem->WaitGPU();

	/** Destroy the world, releasing all actors/component resources */
	World->Destroy();

	/** Wait the GPU/render thread, don't destroy it yet */
	FlushRenderCommands();

	/** Destroy mesh pass */
	CMeshPass::DestroyMeshPasses();

	/** Destroy all basic shaders */
	CBasicShaderClass::DestroyBasicShaders();

	/** Destroy all shader classes */
	CShaderClass::DestroyShaderClasses();

#ifdef DEVELOPMENT
	CStatThread::Stop();
#endif

	ImGui_ImplSDL2_Shutdown();

	LOG(ELogSeverity::Debug, "Destroying engine")
}

void CEngine::FlushRenderCommands()
{
	CRenderThread::Get().GetRenderThreadSemaphore().Wait();
}