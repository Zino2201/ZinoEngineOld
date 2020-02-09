#include "Engine.h"
#include "Render/Window.h"
#include <SDL2/SDL.h>
#include "Render/RenderSystem/Vulkan/VulkanRenderSystem.h"
#include "IO/IOUtils.h"
#include "Render/Commands/RenderCommands.h"
#include "Render/Renderer/ForwardRenderer.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "AssetManager.h"
#include "Render/Material/Material.h"
#include <stb_image.h>
#include "Render/Texture2D.h"
#include "Render/StaticMesh.h"
#include "World/World.h"
#include "World/StaticMeshActor.h"
#include "World/Components/StaticMeshComponent.h"
#include "Render/ShaderAttributesManager.h"
#include "UI/ImGui.h"
#include "Render/Shader.h"
#include <glslang/Public/ShaderLang.h>
#include "Render/Shaders/BasicShader.h"
#include "Render/Shaders/EpilepsieShader.h"
#include "Render/Commands/RenderCommands.h"
#include "RenderThread.h"

//#include "ThirdParty/imgui.h"

CEngine::CEngine() : bHasBeenInitialized(false) {}
CEngine::~CEngine() { }

void CEngine::Initialize()
{
	if (bHasBeenInitialized)
	{
		LOG(ELogSeverity::Error, "Engine already initialized")
		return;
	}
	
	bHasBeenInitialized = true;

	glslang::InitializeProcess();

	Window = std::make_unique<CWindow>(1280, 720, "ZinoEngine");
	AssetManager = std::make_unique<CAssetManager>();
	RenderSystem = std::make_unique<CVulkanRenderSystem>();
	RenderSystem->Initialize();
	Renderer = std::make_unique<CForwardRenderer>();
	MainCommandList = std::make_unique<CRenderCommandList>();
	World = std::make_unique<CWorld>();
	SceneProxy = std::make_unique<CSceneProxy>();

	Loop();
}

void CEngine::InitImGui()
{
	ImGui = std::make_unique<CImGui>();
}

void CEngine::Loop()
{
	CBasicShaderClass::InstantiateBasicShaders();

	LOG(ELogSeverity::Info, "Shader classes: %d", CShaderClass::GetShaderClassMap().size())

	/** Material test */
	std::shared_ptr<CMaterial> Material = AssetManager->Get<CMaterial>("Materials/test.json");

	/** Load texture */
	std::shared_ptr<CTexture2D> Texture = AssetManager->Get<CTexture2D>("dragon.jpg");
	Material->SetTexture(EShaderStage::Fragment,
		"TexSampler",
		Texture->GetTextureView());

	std::shared_ptr<CStaticMesh> Mesh = AssetManager->Get<CStaticMesh>("dragon.obj");

	for(int i = 0; i < 2; ++i)
	{
		std::shared_ptr<CStaticMeshActor> Actor = World->SpawnActor<CStaticMeshActor>(
			STransform(glm::dvec3(0.f, 0.f, i * 3)));
		Actor->GetStaticMesh()->SetStaticMesh(Mesh);
		Actor->GetStaticMesh()->SetMaterial(Material);
	}

	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 LightPos = glm::vec3(0, 5, 10);
	glm::vec3 CameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 CameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 CameraDirection = glm::normalize(CameraPos - CameraTarget);
	glm::vec3 CameraRight = glm::normalize(glm::cross(Up, CameraDirection));
	glm::vec3 CameraUp = glm::cross(CameraDirection, CameraRight);
	glm::vec3 CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

	std::array<float, 4> ClearColor = { 0.f, 0.f, 0.f, 1.0f };
	
	//InitImGui();

	/** Should threads continue ticking */
	Run = true;

	float CamYaw = 0.f, CamPitch = 0.f;
	bool bIsMouseGrabbed = true;

	SDL_WarpMouseInWindow(Window->GetSDLWindow(), Window->GetWidth() / 2, Window->GetHeight() / 2);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	/** test */
	CEpilepsieShaderVS* VS = GetBasicShaderByName<CEpilepsieShaderVS>("EpilepsieShaderVS");
	CEpilepsieShaderFS* FS = GetBasicShaderByName<CEpilepsieShaderFS>("EpilepsieShaderFS");
	IRenderSystemGraphicsPipelinePtr Pipeline = RenderSystem->CreateGraphicsPipeline(
		SRenderSystemGraphicsPipelineInfos(
			VS->GetShader(),
			FS->GetShader(),
			SVertex::GetBindingDescription(),
			SVertex::GetAttributeDescriptions(),
			{}));

	/** Simple quad for epilepsie */
	CRenderSystemBufferPtr VertexBuffer;
	std::array<SVertex, 4> Vertices =
	{
		SVertex(glm::vec3(-1, 1, 0)),
		SVertex(glm::vec3(1, 1, 0)),
		SVertex(glm::vec3(-1, -1, 0)),
		SVertex(glm::vec3(1, -1, 0)),
	};

	{
		CRenderSystemBufferPtr StagingBuffer = RenderSystem->CreateBuffer(
			SRenderSystemBufferInfos(
				sizeof(Vertices[0]) * Vertices.size(),
				EBufferUsage::TransferSrc,
				EBufferMemoryUsage::CpuToGpu));

		void* Data = StagingBuffer->Map();
		memcpy(Data, Vertices.data(), sizeof(Vertices[0]) * Vertices.size());
		StagingBuffer->Unmap();

		VertexBuffer = RenderSystem->CreateBuffer(
			SRenderSystemBufferInfos(
				sizeof(Vertices[0]) * Vertices.size(),
				EBufferUsage::VertexBuffer | EBufferUsage::TransferDst,
				EBufferMemoryUsage::GpuOnly));

		VertexBuffer->Copy(StagingBuffer.get());
	}

	/** Main thread = game thread */
	while (Run)
	{
		/** Delta time */
		static auto StartTime = std::chrono::high_resolution_clock::now();

		auto CurrentTime = std::chrono::high_resolution_clock::now();
		float DeltaTime = std::chrono::duration<float, std::chrono::seconds::period>
			(CurrentTime - StartTime).count();

		float Sensitivity = 0.25f;

		/** Event handling */

		float CameraSpeed = 0.00015f * DeltaTime;

		SDL_Event Event;
		while (SDL_PollEvent(&Event))
		{
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
				Run = false;
				RenderSystem->WaitGPU();
				return;
			}
			
			if(Event.type == SDL_MOUSEMOTION && bIsMouseGrabbed)
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

		const Uint8* KeyState = SDL_GetKeyboardState(nullptr);
		Uint32 MouseState = SDL_GetMouseState(nullptr, nullptr);

		if(MouseState & SDL_BUTTON_LMASK 
			&& !bIsMouseGrabbed)
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

		World->Tick(DeltaTime);

		/** Tell render thread to render scene */
		DrawScene();

		//Renderer->GetMainCommandList()->ClearQueue();

		//Renderer->GetMainCommandList()->Enqueue<CRenderCommandBeginRecording>();
		//Renderer->GetMainCommandList()->Enqueue<CRenderCommandBeginRenderPass>(ClearColor);

		//Renderer->GetMainCommandList()->Enqueue<CRenderCommandBindGraphicsPipeline>(
		//	Material->GetPipeline());

		//Material->SetVec3("Ambient", glm::vec3(.0215f, .1745f, .0215f));
		//Material->SetVec3("Diffuse", glm::vec3(.07568f, .61424f, .07568f));
		//Material->SetVec3("Specular", glm::vec3(.633f, .727811f, .633f));
		//Material->SetFloat("Shininess", 76.8f);

		//Renderer->GetMainCommandList()->Enqueue<CRenderCommandBindShaderAttributeManager>(
		//	Material->GetShaderAttributesManager());

		//LightPos = glm::vec3(0, 0, -500);

		///** Create render commands using game state */
		//for (const std::shared_ptr<CActor>& Actor : World->GetActors())
		//{
		//	/** Get static mesh components */
		//	auto MeshComponents = Actor->GetComponentsByClass<CStaticMeshComponent>();
		//	for (const std::weak_ptr<CStaticMeshComponent>& MeshComponentWeak : MeshComponents)
		//	{
		//		std::shared_ptr<CStaticMeshComponent> MeshComponent = MeshComponentWeak.lock();

		//		std::shared_ptr<CStaticMesh> Mesh = MeshComponent->GetStaticMesh();
		//		MeshComponent->UBO.World = glm::translate(glm::mat4(1.f),
		//			MeshComponent->GetTransform().Position);
		//		MeshComponent->UBO.View = glm::lookAt(CameraPos, CameraPos + CameraFront, 
		//			CameraUp);
		//		MeshComponent->UBO.Projection = glm::perspective(glm::radians(45.f),
		//			(float)Window->GetWidth() / (float)Window->GetHeight(), 0.1f, 10000.0f);
		//		MeshComponent->UBO.Projection[1][1] *= -1;
		//		MeshComponent->UBO.CamPos = CameraPos;

		//		MeshComponent->GetShaderAttributesManager()->SetUniformBuffer("LightUBO",
		//			&LightPos);

		//		MeshComponent->GetShaderAttributesManager()->SetUniformBuffer("UBO", 
		//			&MeshComponent->UBO);
		//		
		//		Renderer->GetMainCommandList()->Enqueue<CRenderCommandBindShaderAttributeManager>(
		//			MeshComponent->GetShaderAttributesManager());

		//		Renderer->GetMainCommandList()->Enqueue<CRenderCommandBindVertexBuffers>(
		//			Mesh->GetVertexBuffer());
		//		Renderer->GetMainCommandList()->Enqueue<CRenderCommandBindIndexBuffer>(
		//			Mesh->GetIndexBuffer(), 0,
		//			Mesh->GetOptimalIndexFormat());
		//		Renderer->GetMainCommandList()->Enqueue<CRenderCommandDrawIndexed>(
		//			Mesh->GetIndexCount(), 1, 0, 0, 0);
		//	}
		//}

		////ImGui::ShowTestWindow();

		///** Epilepsie quad */
		//Renderer->GetMainCommandList()->Enqueue<CRenderCommandBindGraphicsPipeline>(
		//	Pipeline.get());

		//Renderer->GetMainCommandList()->Enqueue<CRenderCommandBindVertexBuffers>(
		//	VertexBuffer.get());

		//Renderer->GetMainCommandList()->Enqueue<CRenderCommandDraw>(
		//	4, 1, 0, 0);

		//Renderer->GetMainCommandList()->Enqueue<CRenderCommandEndRenderPass>();
		//Renderer->GetMainCommandList()->Enqueue<CRenderCommandEndRecording>();

		/** Execute commands */
		

		/** Execute all commands */
	/*	while (Renderer->GetMainCommandList()->GetCommandsCount() > 0)
		{
			Renderer->GetMainCommandList()->ExecuteFrontCommand();
		}*/

		
	}
}

void CEngine::DrawScene()
{
	if(!RenderThread.joinable())
		RenderThread = std::thread(&CEngine::RenderThreadMain, this);
}

void CEngine::RenderThreadMain()
{
	while(true)
	{
		MainCommandList->ClearQueue();

		RenderSystem->Prepare();
		MainCommandList->Enqueue<CRenderCommandBeginRecording>();
		Renderer->Render(MainCommandList.get());
		MainCommandList->Enqueue<CRenderCommandEndRecording>();

		/** Execute all commands */
		while (MainCommandList->GetCommandsCount() > 0)
		{
			MainCommandList->ExecuteFrontCommand();
		}

		RenderSystem->Present();
	}
}