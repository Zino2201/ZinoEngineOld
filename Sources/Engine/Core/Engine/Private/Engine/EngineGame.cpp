#include "Engine/EngineGame.h"
#include "Render/Window.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/RenderSystem/RenderSystemContext.h"
#include "Engine/Viewport.h"
#include "Render/RenderThread.h"
#include <array>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Engine/Assets/StaticMesh.h"
#include <SDL2/SDL.h>
#include "Engine/World.h"
#include "Engine/ECS.h"
#include "Engine/Components/TransformComponent.h"
#include "Engine/Components/StaticMeshComponent.h"
#include "Renderer/ClusteredForward/ClusteredForwardWorldRenderer.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include "Pool.h"
#include <sstream>
//s

namespace ZE
{

CEngineGame::~CEngineGame() { testSM.reset(); delete Window; }

void LoadModelUsingTinyObjLoader(const std::string_view& InPath,
	std::vector<SStaticMeshVertex>& OutVertices,
	std::vector<uint32_t>& OutIndices)
{
	tinyobj::attrib_t Attrib;
	std::vector<tinyobj::shape_t> Shapes;
	std::vector<tinyobj::material_t> Materials;
	std::string Warn, Err;

	if (!tinyobj::LoadObj(&Attrib, &Shapes, &Materials, &Warn, &Err, InPath.data()))
		return;

	std::unordered_map<SStaticMeshVertex, uint32_t, SStaticMeshVertexHash> UniqueVertices;

	for (const auto& Shape : Shapes)
	{
		for (const auto& Index : Shape.mesh.indices)
		{
			SStaticMeshVertex Vertex;
			Vertex.Position.X = Attrib.vertices[3 * Index.vertex_index + 0];
			Vertex.Position.Y = Attrib.vertices[3 * Index.vertex_index + 1];
			Vertex.Position.Z = Attrib.vertices[3 * Index.vertex_index + 2];

			Vertex.Normal.X = Attrib.normals[3 * Index.normal_index + 0];
			Vertex.Normal.Y = Attrib.normals[3 * Index.normal_index + 1];
			Vertex.Normal.Z = Attrib.normals[3 * Index.normal_index + 2];

			if (UniqueVertices.count(Vertex) == 0)
			{
				UniqueVertices[Vertex] = static_cast<uint32_t>(OutVertices.size());
				OutVertices.push_back(Vertex);
			}

			OutIndices.push_back(UniqueVertices[Vertex]);
		}
	}
}

void LoadModelUsingAssimp(const std::string_view& InPath,
	std::vector<SStaticMeshVertex>& OutVertices,
	std::vector<uint32_t>& OutIndices)
{
	static std::vector<Math::SVector3Float> Colors = 
	{
		Math::SVector3Float(1, 0, 0),
		Math::SVector3Float(0, 1, 0),
		Math::SVector3Float(0, 0, 1)
	};
	static size_t CurrentColor = 0;
	static size_t CurrentVtx = 0;

	Assimp::Importer Importer;
	const aiScene* Scene = Importer.ReadFile(InPath.data(), aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);
	if(!Scene)
		LOG(ELogSeverity::Fatal, None, "Failed to load model %s", InPath.data());

	aiMesh* Mesh = Scene->mMeshes[0];

	OutVertices.reserve(Mesh->mNumVertices);
	
	std::unordered_map<SStaticMeshVertex, uint32_t, SStaticMeshVertexHash> UniqueVertices;

	for(uint32_t i = 0; i < Mesh->mNumVertices; ++i)
	{
		aiVector3D& Position = Mesh->mVertices[i];
		aiVector3D& Normal = Mesh->mNormals[i];

		SStaticMeshVertex Vertex;
		Vertex.Position = { Position.x, Position.y, Position.z };
		Vertex.Normal = { Normal.x, Normal.y, Normal.z };

		if(UniqueVertices.count(Vertex) == 0)
		{
			UniqueVertices[Vertex] = static_cast<uint32_t>(OutVertices.size());
			CurrentVtx++;
			if(CurrentVtx == 32)
			{
				CurrentColor = (CurrentColor + 1) % Colors.size();
				CurrentVtx = 0;
			}
			OutVertices.push_back(Vertex);
		}

		OutIndices.emplace_back(UniqueVertices[Vertex]);
	}

}

struct test
{
	alignas(16) glm::mat4 View;
	alignas(16) glm::vec3 CameraPos;
	alignas(16) glm::vec3 CameraFront;
	alignas(16) glm::vec3 CameraUp;
};

int StaticOnWindowResized(void* InUserData, SDL_Event* InEvent)
{
	must(InUserData);

	return reinterpret_cast<CEngineGame*>(InUserData)->OnWindowResized(InEvent);
}

#include <cstdlib>
#include <chrono>
#define RAND_1_0 (static_cast<double>(rand()) / static_cast <double> (RAND_MAX))

void CEngineGame::Initialize()
{
	CEngine::Initialize();

	srand(2201);

	using namespace ZE::Components;

	/** Create main game window */
	Window = new CWindow("ZinoEngine", 1280, 720);

	/** Event on resize */
	SDL_AddEventWatch(&StaticOnWindowResized, this);

	Viewport = std::make_unique<CViewport>(Window->GetHandle(), Window->GetWidth(), 
		Window->GetHeight());
	Viewport->InitResource();

	World = std::make_unique<CWorld>();

	/**
	 * Read merger sponger
	 */
	std::vector<SStaticMeshVertex> Vertices;
	std::vector<uint32_t> Indices;
	LoadModelUsingTinyObjLoader("Assets/sphere.obj", Vertices, Indices);

	testSM = std::make_shared<CStaticMesh>();
	testSM->UpdateData(Vertices, Indices);

	for(int i = 0; i < 105; ++i)
	{
		double X = RAND_1_0 * 100 - 50;
		double Y = RAND_1_0 * 100 - 50;
		double Z = RAND_1_0 * -300;

		ECS::EntityID Entity = World->GetEntityManager()->CreateEntity();
		Components::STransformComponent* T = 
			World->GetEntityManager()->AddComponent<Components::STransformComponent>(Entity);
		T->Transform.Position.X = X;
		T->Transform.Position.Y = Y;
		T->Transform.Position.Z = Z;

		Components::SStaticMeshComponent* SM = 
			World->GetEntityManager()->AddComponent<Components::SStaticMeshComponent>(Entity);
		SM->SetStaticMesh(testSM);
	}

	ViewDataUBO.InitResource();
}

glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 LightPos = glm::vec3(0, 5, 10);
glm::vec3 CameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 CameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 CameraDirection = glm::normalize(CameraPos - CameraTarget);
glm::vec3 CameraRight = glm::normalize(glm::cross(Up, CameraDirection));
glm::vec3 CameraUp = glm::cross(CameraDirection, CameraRight);
glm::vec3 CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	float CamYaw = 0.f, CamPitch = 0.f;

int CEngineGame::OnWindowResized(SDL_Event* InEvent)
{
	switch(InEvent->window.event)
	{
	case SDL_WINDOWEVENT_SIZE_CHANGED:
	case SDL_WINDOWEVENT_RESIZED:
		Window->SetWidth(static_cast<uint32_t>(InEvent->window.data1));
		Window->SetHeight(static_cast<uint32_t>(InEvent->window.data2));
		
		// Resize viewport
		EnqueueRenderCommand("ResizeWindow",
			[this]()
		{
			Viewport->Resize(Window->GetWidth(), Window->GetHeight());
		});

		return SDL_TRUE;
	}

	return SDL_FALSE;
}

void CEngineGame::Tick(SDL_Event* InEvent, const float& InDeltaTime)
{
	CEngine::Tick(InEvent, InDeltaTime);

	static Uint64 Now = SDL_GetPerformanceCounter();
	static Uint64 Last = 0;

	Last = Now;
	Now = SDL_GetPerformanceCounter();

	float DeltaTime = (((Now - Last) * 1000 / (float)SDL_GetPerformanceFrequency()))
		* 0.001f;

	/** Update camera */
	const Uint8* KeyState = SDL_GetKeyboardState(nullptr);
	Uint32 MouseState = SDL_GetMouseState(nullptr, nullptr);
	static bool bIsMouseGrabbed = false;
	const float CameraSpeed = 5.f * DeltaTime;

	if (MouseState & SDL_BUTTON_LMASK
		&& !bIsMouseGrabbed)
	{
		SDL_SetRelativeMouseMode(SDL_TRUE);
		bIsMouseGrabbed = true;
	}
	if (KeyState[SDL_SCANCODE_W])
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

	if (InEvent->type == SDL_MOUSEMOTION)
	{
		if (bIsMouseGrabbed)
		{
			float DeltaX = InEvent->motion.xrel * 0.5f;
			float DeltaY = InEvent->motion.yrel * 0.5f;

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
			Front.y *= -1;
			CameraFront = glm::normalize(Front);

			SDL_WarpMouseInWindow((SDL_Window*) Window->GetHandle(),
				Window->GetWidth() / 2, Window->GetHeight() / 2);
		}
	}

	/** Draw the scene */
	EnqueueRenderCommand("CEngineGame::DrawWorld",
		[this, DeltaTime]()
	{
		/**
		 * Don't render if begin fail (swapchain recreated)
		 */
		if(Viewport->Begin())
		{
			{
				ZE::Renderer::SViewData VD;
				glm::mat4 View = glm::lookAt(CameraPos, CameraPos
					+ CameraFront,
					CameraUp);
				glm::mat4 Proj = glm::perspective(glm::radians(90.f),
					(float)Window->GetWidth() / Window->GetHeight(),
					100000.F, 0.01f);
				VD.ViewProj = Proj * View;
				VD.ViewPos = CameraPos;
				VD.ViewForward = CameraFront;
				ViewDataUBO.Copy(&VD);
			}

			ZE::Renderer::SWorldRendererView View;
			View.Width = Window->GetWidth();
			View.Height = Window->GetHeight();
			View.Surface = Viewport->GetSurface();
			View.ViewDataUBO = ViewDataUBO.GetBuffer();
			ZE::Renderer::CClusteredForwardWorldRenderer Renderer;

			Renderer.Render(World->GetProxy(), View);
			Viewport->End();
		}
	});

	static float fpsC = 0;
	if(fpsC >= 0.15f)
	{
		std::stringstream NewTitle;
		NewTitle << std::fixed;
		NewTitle << "ZinoEngine | FPS: " << (int)1.f / DeltaTime << " (" << DeltaTime << " ms)";
		SDL_SetWindowTitle(reinterpret_cast<SDL_Window*>(Window->GetHandle()),
			NewTitle.str().c_str());
		fpsC = 0;
	}
	fpsC += DeltaTime;

}

void CEngineGame::Exit()
{
	testSM->GetRenderData()->DestroyResource();
}

CEngine* CreateEngine()
{
	return new CEngineGame;
}

} /* namespace ZE */