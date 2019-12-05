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
#include "AssetManager.h"
#include "Render/Material.h"
#include <stb_image.h>
#include "Render/Texture2D.h"

struct STestUBO
{
	glm::mat4 World;
	glm::mat4 View;
	glm::mat4 Projection;
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
	AssetManager = std::make_unique<CAssetManager>();
	RenderSystem = std::make_unique<CVulkanRenderSystem>();
	RenderSystem->Initialize();
	Renderer = std::make_unique<CRenderer>();

	Loop();
}

void CEngine::Loop()
{
	const std::vector<SVertex> Vertices = 
	{
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};

	const std::vector<uint16_t> Indices = 
	{
		0, 1, 2, 2, 3, 0
	};

	std::shared_ptr<IShader> Vertex = g_VulkanRenderSystem->CreateShader(
		IOUtils::ReadBinaryFile("Assets/Shaders/main.vert.spv"),
		EShaderStage::Vertex);

	std::shared_ptr<IShader> Fragment = g_VulkanRenderSystem->CreateShader(
		IOUtils::ReadBinaryFile("Assets/Shaders/main.frag.spv"),
		EShaderStage::Fragment);

	/** Uniform buffer & pipeline */
	std::shared_ptr<IUniformBuffer> UniformBuffer = g_VulkanRenderSystem->CreateUniformBuffer(
		SUniformBufferInfos(sizeof(STestUBO)));

	/** Material test */
	std::shared_ptr<CMaterial> Material = AssetManager->Get<CMaterial>("Materials/test.json");

	/** Create vertex buffer using staging buffer */
	std::shared_ptr<IBuffer> VertexBuffer;
	{
		std::shared_ptr<IBuffer> StagingBuffer = g_VulkanRenderSystem->CreateBuffer(
			SBufferInfos(
				sizeof(Vertices[0]) * Vertices.size(),
				EBufferUsage::TransferSrc,
				EBufferMemoryUsage::CpuToGpu));

		void* Data = StagingBuffer->Map();
		memcpy(Data, Vertices.data(), sizeof(Vertices[0]) * Vertices.size());
		StagingBuffer->Unmap();

		VertexBuffer = g_VulkanRenderSystem->CreateBuffer(
			SBufferInfos(
				sizeof(Vertices[0]) * Vertices.size(),
				EBufferUsage::VertexBuffer | EBufferUsage::TransferDst,
				EBufferMemoryUsage::GpuOnly));

		StagingBuffer->Copy(VertexBuffer.get());
	}

	/** Create index buffer using staging buffer */
	std::shared_ptr<IBuffer> IndexBuffer;
	{
		std::shared_ptr<IBuffer> StagingBuffer = g_VulkanRenderSystem->CreateBuffer(
			SBufferInfos(
				sizeof(Indices[0]) * Indices.size(),
				EBufferUsage::TransferSrc,
				EBufferMemoryUsage::CpuToGpu));

		void* Data = StagingBuffer->Map();
		memcpy(Data, Indices.data(), sizeof(Indices[0]) * Indices.size());
		StagingBuffer->Unmap();

		IndexBuffer = g_VulkanRenderSystem->CreateBuffer(
			SBufferInfos(
				sizeof(Indices[0]) * Indices.size(),
				EBufferUsage::IndexBuffer | EBufferUsage::TransferDst,
				EBufferMemoryUsage::GpuOnly));

		StagingBuffer->Copy(IndexBuffer.get());
	}

	/** Load texture */
	std::shared_ptr<CTexture2D> Texture = AssetManager->Get<CTexture2D>("Textures/Pepsiman.jpg");

	std::array<float, 4> ClearColor = { 0.f, 0.f, 0.f, 1.0f };

	/** Should threads continue ticking */
	Run = true;

	Material->SetShaderAttributeResource(
		EShaderStage::Vertex,
		"UBO", 
		UniformBuffer->GetBuffer());

	Material->SetShaderAttributeResource(
		EShaderStage::Fragment,
		"TexSampler",
		Texture->GetTextureView());

	// TODO: Game state

	RenderThread = std::thread([this, &ClearColor, &VertexBuffer, &IndexBuffer, &Vertices,
		&Indices, &Material]
	{
		
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
				break;
			case SDL_QUIT:
				Run = false;
				RenderThread.join();
				RenderSystem->WaitGPU();
				return;
			}
		}

		/** Tick */
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time 
			= std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime)
			.count();

		STestUBO UBO;
		UBO.World = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), 
			glm::vec3(0.0f, 0.0f, 1.0f));
		UBO.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), 
			glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		UBO.Projection = glm::perspective(glm::radians(45.0f),
			Window->GetWidth() / (float) Window->GetHeight(), 0.1f, 10.0f);
		UBO.Projection[1][1] *= -1;

		memcpy(UniformBuffer->GetMappedMemory(), &UBO, sizeof(UBO));

		Renderer->GetMainCommandList()->ClearQueue();

		Renderer->GetMainCommandList()->Enqueue<CRenderCommandBeginRecording>();
		Renderer->GetMainCommandList()->Enqueue<CRenderCommandBeginRenderPass>(ClearColor);
		Renderer->GetMainCommandList()->Enqueue<CRenderCommandBindGraphicsPipeline>(
			Material->GetPipeline());
		Renderer->GetMainCommandList()->Enqueue<CRenderCommandBindVertexBuffers>(VertexBuffer);
		Renderer->GetMainCommandList()->Enqueue<CRenderCommandBindIndexBuffer>(IndexBuffer, 0,
			EIndexFormat::Uint16);
		Renderer->GetMainCommandList()->Enqueue<CRenderCommandDrawIndexed>(
			static_cast<uint32_t>(Indices.size()), 1, 0, 0, 0);
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
}