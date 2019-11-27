#include "Engine.h"
#include "Render/Window.h"
#include <SDL2/SDL.h>
#include "Render/Vulkan/VulkanRenderSystem.h"
#include "IO/IOUtils.h"
#include "Render/Pipeline.h"
#include "Render/Shader.h"
#include "Render/Commands/RenderCommands.h"
#include "Render/Renderer.h"

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

	Window = std::make_unique<CWindow>(1280, 720);
	RenderSystem = std::make_unique<CVulkanRenderSystem>();
	RenderSystem->Initialize();
	Renderer = std::make_unique<CRenderer>();

	Loop();
}

void CEngine::Loop()
{
	std::shared_ptr<IShader> Vertex = g_VulkanRenderSystem->CreateShader(
		IOUtils::ReadBinaryFile("Assets/Shaders/main.vert.spv"),
		EShaderStage::Vertex);

	std::shared_ptr<IShader> Fragment = g_VulkanRenderSystem->CreateShader(
		IOUtils::ReadBinaryFile("Assets/Shaders/main.frag.spv"),
		EShaderStage::Fragment);

	std::shared_ptr<IGraphicsPipeline> Pipeline = g_VulkanRenderSystem->CreateGraphicsPipeline(
		Vertex.get(),
		Fragment.get());

	while (true)
	{
		/** Event handling */

		SDL_Event Event;
		while (SDL_PollEvent(&Event))
		{
			switch (Event.type)
			{
			case SDL_QUIT:
				RenderSystem->PrepareDestroy();
				return;
			}
		}

		/** Tick */

		/** Render */
		std::array<float, 4> ClearColor = { 0.f, 0.f, 0.f, 1.0f };
		
		RenderSystem->Prepare();
		
		Renderer->GetMainCommandList()->Enqueue<CRenderCommandBeginRecording>();
		Renderer->GetMainCommandList()->Enqueue<CRenderCommandBeginRenderPass>(ClearColor);
		Renderer->GetMainCommandList()->Enqueue<CRenderCommandBindGraphicsPipeline>(Pipeline);
		Renderer->GetMainCommandList()->Enqueue<CRenderCommandDraw>(3, 1, 0, 0);
		Renderer->GetMainCommandList()->Enqueue<CRenderCommandEndRenderPass>();
		Renderer->GetMainCommandList()->Enqueue<CRenderCommandEndRecording>();
		Renderer->GetMainCommandList()->ExecuteAndFlush();

		RenderSystem->Present();
	}
}