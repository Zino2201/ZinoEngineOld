#include "Engine.h"
#include "Render/Window.h"
#include <SDL2/SDL.h>
#include "Render/Vulkan/VulkanRenderSystem.h"
#include "IO/IOUtils.h"
#include "Render/Pipeline.h"
#include "Render/Shader.h"
#include "Render/Commands/RenderCommands.h"
#include "Render/Renderer.h"
#include "Render/Buffer.h"

struct SVertex
{
	glm::vec2 Position;
	glm::vec3 Color;

	static SVertexInputBindingDescription GetBindingDescription()
	{
		return SVertexInputBindingDescription(0, sizeof(SVertex), EVertexInputRate::Vertex);
	}

	static std::vector<SVertexInputAttributeDescription> GetAttributeDescriptions()
	{
		return
		{
			SVertexInputAttributeDescription(0, 0, EFormat::R32G32Sfloat,
				offsetof(SVertex, Position)),
			SVertexInputAttributeDescription(0, 1, EFormat::R32G32B32Sfloat, 
				offsetof(SVertex, Color)),
		};
	}
};

CEngine::CEngine() : bHasBeenInitialized(false) {}
CEngine::~CEngine() {}

void CEngine::Initialize()
{
	if (bHasBeenInitialized)
	{
		LOG(ELogSeverity::Error, "Engine already initialized")
		return;
	}
	
	bHasBeenInitialized = true;

	Window = std::make_unique<CWindow>(1280, 720, "ZinoEngine");
	RenderSystem = std::make_unique<CVulkanRenderSystem>();
	RenderSystem->Initialize();
	Renderer = std::make_unique<CRenderer>();

	Loop();
}

void CEngine::Loop()
{
	const std::vector<SVertex> Vertices = 
	{
		{{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};

	std::shared_ptr<IShader> Vertex = g_VulkanRenderSystem->CreateShader(
		IOUtils::ReadBinaryFile("Assets/Shaders/main.vert.spv"),
		EShaderStage::Vertex);

	std::shared_ptr<IShader> Fragment = g_VulkanRenderSystem->CreateShader(
		IOUtils::ReadBinaryFile("Assets/Shaders/main.frag.spv"),
		EShaderStage::Fragment);

	std::shared_ptr<IGraphicsPipeline> Pipeline = g_VulkanRenderSystem->CreateGraphicsPipeline(
		Vertex.get(),
		Fragment.get(),
		SVertex::GetBindingDescription(),
		SVertex::GetAttributeDescriptions());

	std::shared_ptr<IBuffer> VertexBuffer = g_VulkanRenderSystem->CreateBuffer(
		SBufferInfos(
			sizeof(Vertices[0]) * Vertices.size(), 
			EBufferUsage::VertexBuffer,
			EBufferMemoryUsage::CpuToGpu));

	void* Data = VertexBuffer->Map();
	memcpy(Data, Vertices.data(), sizeof(Vertices[0]) * Vertices.size());
	VertexBuffer->Unmap();

	std::array<float, 4> ClearColor = { 0.f, 0.f, 0.f, 1.0f };

	/** Should threads continue ticking */
	Run = true;

	// TODO: Game state

	RenderThread = std::thread([this, &ClearColor, &Pipeline, &VertexBuffer, &Vertices]
	{
		while (Run)
		{
			Renderer->GetMainCommandList()->ClearQueue();

			Renderer->GetMainCommandList()->Enqueue<CRenderCommandBeginRecording>();
			Renderer->GetMainCommandList()->Enqueue<CRenderCommandBeginRenderPass>(ClearColor);
			Renderer->GetMainCommandList()->Enqueue<CRenderCommandBindGraphicsPipeline>(Pipeline);
			Renderer->GetMainCommandList()->Enqueue<CRenderCommandBindVertexBuffers>(VertexBuffer);
			Renderer->GetMainCommandList()->Enqueue<CRenderCommandDraw>(
				static_cast<uint32_t>(Vertices.size()), 1, 0, 0);
			Renderer->GetMainCommandList()->Enqueue<CRenderCommandEndRenderPass>();
			Renderer->GetMainCommandList()->Enqueue<CRenderCommandEndRecording>();

			RenderSystem->Prepare();
			/** Execute all commands */
			while (Renderer->GetMainCommandList()->GetCommandsCount() > 0)
			{
				Renderer->GetMainCommandList()->ExecuteFrontCommand();
			}

			RenderSystem->Present();
		}
	});

	/** Main thread = game thread */
	while (Run)
	{
		/** Event handling */
		SDL_Event Event;
		while (SDL_PollEvent(&Event))
		{
			switch (Event.type)
			{
			case SDL_WINDOWEVENT:
				if (Event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					int NewWidth = 0;
					int NewHeight = 0;
					SDL_GetWindowSize(Window->GetSDLWindow(), &NewWidth, &NewHeight);
					Window->SetWidth(NewWidth);
					Window->SetHeight(NewHeight);
					Window->OnWindowResized.Broadcast();
				}
				else if (Event.window.event == SDL_WINDOWEVENT_MINIMIZED)
				{
					Uint32 Flags = SDL_GetWindowFlags(Window->GetSDLWindow());
					while (Flags & SDL_WINDOW_MINIMIZED)
					{
						Flags = SDL_GetWindowFlags(Window->GetSDLWindow());
						SDL_WaitEvent(nullptr);
					}
				}
				break;
			case SDL_QUIT:
				Run = false;
				RenderThread.join();
				RenderSystem->WaitGPU();
				return;
			}
		}
	}
}