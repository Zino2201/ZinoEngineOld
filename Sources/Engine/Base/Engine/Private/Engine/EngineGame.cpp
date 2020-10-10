#include "Engine/EngineGame.h"
#include "Render/Window.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/RenderSystem/Resources/Surface.h"
#include "Render/RenderSystem/RenderSystemContext.h"
#include "Engine/Viewport.h"
#include <array>
#include "Engine/Assets/StaticMesh.h"
#include <SDL.h>
#include "Engine/World.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include "Pool.h"
#include <sstream>
#include "ImGui/ImGui.h"
#include <examples/imgui_impl_sdl.h>
#include "Profiling/Profiling.h"
#include "Threading/JobSystem/JobSystem.h"
#include <iostream>
#include <chrono>
#include "Renderer/RendererModule.h"
#include "Engine/UI/Console.h"
#include "Engine/InputSystem.h"

namespace ZE
{

static TConVar<int32_t> ConVSync("r_vsync", 0, 
	"Enable V-SYNC.\n0 = disabled\n1 = enabled\n2 = triple buffering (not implemented)");

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

	return reinterpret_cast<CZEGameApp*>(InUserData)->OnWindowResized(InEvent);
}

#include <cstdlib>
#include <chrono>
#define RAND_1_0 (static_cast<double>(rand()) / static_cast <double> (RAND_MAX))

CZEGameApp::CZEGameApp() : 
	CZinoEngineApp(false)
{
	srand(2201);

	ConVSync.BindOnChanged(this, &CZEGameApp::OnVsyncChanged);

	/** Create main game window */
	Window = std::make_unique<CWindow>("ZinoEngine", 1600, 900, 
		EWindowFlagBits::Centered | EWindowFlagBits::Resizable | EWindowFlagBits::Maximized);

	/** Event on resize */
	SDL_AddEventWatch(&StaticOnWindowResized, this);

	ImGui_ImplSDL2_InitForVulkan(reinterpret_cast<SDL_Window*>(Window->GetHandle()));

	ImGuiIO& IO = ImGui::GetIO();
	IO.DisplaySize = ImVec2(Window->GetWidth(), Window->GetHeight());
	IO.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	IO.WantCaptureKeyboard = true;
	IO.WantCaptureMouse = true;
	Font = IO.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto-Medium.ttf", 16.f);
	
	Viewport = std::make_unique<CViewport>(Window->GetHandle(), Window->GetWidth(), 
		Window->GetHeight(), ConVSync.Get());

	World = std::make_unique<CWorld>();

	/**
	 * Read merger sponger
	 */
	std::vector<SStaticMeshVertex> Vertices;
	std::vector<uint32_t> Indices;
	LoadModelUsingTinyObjLoader("Assets/sphere.obj", Vertices, Indices);

	testSM = std::make_shared<CStaticMesh>();
	testSM->UpdateData(Vertices, Indices);

	Renderer::CRendererModule::Get().CreateImGuiRenderer();

	ImGuiRenderer = std::make_unique<ZE::UI::CImGuiRender>();
}

CZEGameApp::~CZEGameApp() { ImGui_ImplSDL2_Shutdown(); }

glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 LightPos = glm::vec3(0, 5, 10);
glm::vec3 CameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 CameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 CameraDirection = glm::normalize(CameraPos - CameraTarget);
glm::vec3 CameraRight = glm::normalize(glm::cross(Up, CameraDirection));
glm::vec3 CameraUp = glm::cross(CameraDirection, CameraRight);
glm::vec3 CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	float CamYaw = 0.f, CamPitch = 0.f;
CConsoleWidget Console;

void CZEGameApp::OnVsyncChanged(const int32_t& InData)
{
	Viewport->SetVSync(InData);
}

int CZEGameApp::OnWindowResized(SDL_Event* InEvent)
{
	switch(InEvent->window.event)
	{
	case SDL_WINDOWEVENT_SIZE_CHANGED:
	case SDL_WINDOWEVENT_RESIZED:
		Window->SetWidth(static_cast<uint32_t>(InEvent->window.data1));
		Window->SetHeight(static_cast<uint32_t>(InEvent->window.data2));
		
		ImGuiIO& IO = ImGui::GetIO();
		IO.DisplaySize = ImVec2(Window->GetWidth(), Window->GetHeight());

		// Resize viewport
		Viewport->Resize(Window->GetWidth(), Window->GetHeight());

		return SDL_TRUE;
	}

	return SDL_FALSE;
}

static bool bIsMouseGrabbed = false;

void CZEGameApp::ProcessEvent(SDL_Event& InEvent)
{
	CZinoEngineApp::ProcessEvent(InEvent);

	ImGuiIO& IO = ImGui::GetIO();

	ImGui_ImplSDL2_ProcessEvent(&InEvent);

	if (InEvent.type == SDL_MOUSEMOTION)
	{
		if (bIsMouseGrabbed)
		{
			float DeltaX = InEvent.motion.xrel * 0.5f;
			float DeltaY = InEvent.motion.yrel * 0.5f;

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

	
	/** Update camera */
	Uint32 MouseState = SDL_GetMouseState(nullptr, nullptr);

	const bool bImGuiInteract = ImGui::IsAnyItemHovered()
		|| ImGui::IsAnyWindowHovered()
		|| ImGui::IsAnyItemActive()
		|| ImGui::IsMouseDragging(ImGuiMouseButton_Left);

	if (MouseState & SDL_BUTTON_LMASK
		&& !bIsMouseGrabbed
		&& !bImGuiInteract)
	{
		SDL_SetRelativeMouseMode(SDL_TRUE);
		bIsMouseGrabbed = true;
		IO.WantCaptureKeyboard = false;
		IO.WantCaptureMouse = false;
	}

	if (ZE::Input::IsKeyPressed(SDL_SCANCODE_ESCAPE))
	{
		if (bIsMouseGrabbed)
		{
			SDL_SetRelativeMouseMode(SDL_FALSE);
			bIsMouseGrabbed = false;
			IO.WantCaptureKeyboard = true;
			IO.WantCaptureMouse = true;
		}
	}
}

void CZEGameApp::Tick(const float& InDeltaTime)
{
	ImGui_ImplSDL2_NewFrame(reinterpret_cast<SDL_Window*>(Window->GetHandle()));
	
	const float CameraSpeed = 5.f * InDeltaTime;

	/*if(!bImGuiInteract)
	{*/
		if (ZE::Input::IsKeyHeld(SDL_SCANCODE_W))
		{
			CameraPos += CameraSpeed * CameraFront;
		}
		if (ZE::Input::IsKeyHeld(SDL_SCANCODE_S))
		{
			CameraPos -= CameraSpeed * CameraFront;
		}
		if (ZE::Input::IsKeyHeld(SDL_SCANCODE_A))
		{
			CameraPos -= glm::normalize(glm::cross(CameraFront, CameraUp)) * CameraSpeed;
		}
		if (ZE::Input::IsKeyHeld(SDL_SCANCODE_D))
		{
			CameraPos += glm::normalize(glm::cross(CameraFront, CameraUp)) * CameraSpeed;
		}
	//}
	
	ImGui::NewFrame();

#if 1
	/** UI */
	ImGui::PushFont(Font);
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("TopLeftText", nullptr, ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::TextColored(ImVec4(0.75f, 0.75f, 0.75f, 0.75f), 
		"ZinoEngine %s v%s", ZE_CONFIGURATION_NAME, "0.1.0");
	ImGui::End();

	static std::vector<float> FrameTimes;
	static size_t CurrentIdx = 0;
	if(FrameTimes.capacity() == 0)
	{
		FrameTimes.resize(100);
	}

	FrameTimes[CurrentIdx] = InDeltaTime * 1000;
	CurrentIdx = (CurrentIdx + 1) % 99;

	float Avg = 0.f;
	for(const auto& Val : FrameTimes)
		Avg += Val;
	Avg /= FrameTimes.size();

	/** Print last profiling data */
	ImGui::Begin("Debugger");
	ImGui::Text("FPS: %f (%f ms, avg: %f ms)", 1.f / InDeltaTime, InDeltaTime * 1000, Avg);
	ImGui::Separator();
	ImGui::Text("Rendering"); 

	ImGui::Text("VSync: %i", ConVSync.Get());

	ImGui::Separator();
	ImGui::End();
	Console.Draw();
	ImGui::PopFont();
#endif
	ImGui::Render();
}

void CZEGameApp::Draw()
{
	// TODO: Execute jobs while rendering is processing

	/** Ensure all rendering is finished */
	Renderer::CRendererModule::Get().WaitRendering();

	static bool bHasBegun = false;
	
	// TODO: Move
	/** Present */
	if (bHasBegun)
		Viewport->End();

	/** Trigger rendering */
	GRenderSystem->NewFrame();

	if (bHasBegun = Viewport->Begin())
	{
		ZE::Renderer::SWorldView WorldView(*World->GetProxy());
		WorldView.Scissor = { { 0.f, 0.f }, { Window->GetWidth(), Window->GetHeight() } };
		WorldView.Viewport = { WorldView.Scissor, 0.f, 1.0f };
		glm::mat4 View = glm::lookAt(CameraPos, CameraPos
			+ CameraFront,
			CameraUp);
		glm::mat4 Proj = glm::perspective(glm::radians(90.f),
			(float)Window->GetWidth() / Window->GetHeight(),
			100000.F, 0.01f);
		WorldView.ViewProj = Proj * View;
		WorldView.ViewPos = CameraPos;
		WorldView.ViewForward = CameraFront;
		WorldView.TargetRT = Viewport->GetSurface()->GetBackbufferTexture();
		Renderer::CRendererModule::Get().EnqueueView(WorldView);
	}

	Renderer::CRendererModule::Get().FlushViews();
}

CZinoEngineApp* CreateGameApp()
{
	return new CZEGameApp;
}

} /* namespace ZE */