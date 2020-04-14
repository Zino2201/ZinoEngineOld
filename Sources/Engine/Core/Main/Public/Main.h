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
    static ZE::IRenderSystem* RenderSystem;

    /** Engine ptr */
    static ZE::CEngine* Engine;

    static ZE::CRenderThread* RenderThread;
    static ZE::CGlobalShaderCompiler* ShaderCompiler;
    static SDL_Event Event;

    static std::thread RenderThreadHandle;
};