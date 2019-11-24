#include "Engine.h"
#include "Render/Window.h"
#include <SDL2/SDL.h>
#include "Render/Vulkan/VulkanRenderSystem.h"
#include "Render/Vulkan/VulkanPipeline.h"
#include "IO/IOUtils.h"
#include "Render/Vulkan/VulkanShader.h"
#include "Render/Commands/RenderCommandBeginRenderPass.h"
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
	std::shared_ptr<CVulkanShader> Vertex = std::make_shared<CVulkanShader>(
		g_VulkanRenderSystem->GetDevice(),
		vk::ShaderStageFlagBits::eVertex,
		IOUtils::ReadFile("Assets/Shaders/main.vert.spv"));

	std::shared_ptr<CVulkanShader> Fragment = std::make_shared<CVulkanShader>(
		g_VulkanRenderSystem->GetDevice(),
		vk::ShaderStageFlagBits::eFragment,
		IOUtils::ReadFile("Assets/Shaders/main.frag.spv"));

	/* TEST */
	std::unique_ptr<CVulkanRenderPipeline> Pipeline;

	/** TEST */
	Pipeline = std::unique_ptr<CVulkanRenderPipeline>(new CVulkanRenderPipeline(
		g_VulkanRenderSystem->GetDevice(),
		{ Vertex.get(), Fragment.get() }));

	CRenderCommandBeginRenderPass* test =
		Renderer->GetMainCommandList()->Enqueue<CRenderCommandBeginRenderPass>();

	while (true)
	{
		SDL_Event Event;
		while (SDL_PollEvent(&Event))
		{
			switch (Event.type)
			{
			case SDL_QUIT:
				return;
			}
		}
	}
}