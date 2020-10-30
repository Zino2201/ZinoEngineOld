#include "Engine/EngineGame.h"
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

namespace ze
{

GameApp::GameApp() : EngineApp()
{

}

void GameApp::post_tick(const float in_delta_time)
{
	/** Render */
}


} /* namespace ze */