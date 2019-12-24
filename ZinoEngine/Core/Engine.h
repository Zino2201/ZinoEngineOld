#pragma once

#include "EngineCore.h"
#include "GameState.h"

class CWindow;
class IRenderSystem;
class CRenderer;
class CAssetManager;
class CWorld;

/**
 * Engine singleton
 */
class CEngine
{
public:
	static CEngine& Get()
	{
		static CEngine Instance;
		return Instance;
	}

	/**
	 * Initialize engine
	 */
	void Initialize();

	CWindow* GetWindow() const { return Window.get(); }
	IRenderSystem* GetRenderSystem() const { return RenderSystem.get(); }
	CRenderer* GetRenderer() const { return Renderer.get(); }
private:
	void Loop();
public:
	CEngine(const CEngine&) = delete;
	void operator=(const CEngine&) = delete;
private:
	CEngine();
	~CEngine();
private:
	bool bHasBeenInitialized;
	std::unique_ptr<CWindow> Window;
	std::unique_ptr<IRenderSystem> RenderSystem;
	std::unique_ptr<CRenderer> Renderer;
	std::unique_ptr<CAssetManager> AssetManager;
	std::unique_ptr<CWorld> World;
	std::thread RenderThread;
	std::atomic_bool Run;
	SGameState GameState;
	std::mutex GameStateMutex;
};