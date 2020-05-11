#pragma once

#include "Render/RenderThread.h"
#include <SDL2/SDL.h>

namespace ZE
{
class IRenderSystem;
class CEngine;
class CRenderThread;
class CGlobalShaderCompiler;
}

/**
 * Main class
 * Contains the game thread loop
 */
class CZinoEngineMain
{
public:
    /**
     * Initialize musthave modules and other systems
     */
    static void PreInit();

    /**
     * Initialize actuel engine
     */
    static void Init();

    static void Tick(const float& InDeltaTime);

    /**
     * Exit engine
     */
    static void Exit();
private:
    /**
     * Loop
     */
    static void Loop();
private:
    /** Render system ptr */
    inline static std::unique_ptr<ZE::IRenderSystem> RenderSystem;

    /** Engine ptr */
    inline static std::unique_ptr<ZE::CEngine> Engine;

    inline static std::unique_ptr<ZE::CRenderThread> RenderThread;
    inline static std::unique_ptr<ZE::CGlobalShaderCompiler> ShaderCompiler;
    inline static SDL_Event Event;

    inline static std::thread RenderThreadHandle;
};