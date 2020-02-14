#pragma once

#include "EngineCore.h"

class CWindow;
class IRenderSystem;
class CAssetManager;
class CWorld;
class CImGui;
class CRenderCommandList;

/**
 * Engine singleton
 */
class CEngine
{
public:
	/**
	 * Initialize engine
	 */
	void Initialize();

	void FlushRenderCommands();

	void Destroy();

	void Exit();

	CWindow* GetWindow() const { return Window.get(); }
	IRenderSystem* GetRenderSystem() const { return RenderSystem.get(); }
	CAssetManager* GetAssetManager() const { return AssetManager.get(); }
	CWorld* GetWorld() const { return World.get(); }
private:
	void Loop();
	void InitImGui();
public:
	CEngine();
	~CEngine();
private:
	bool bHasBeenInitialized;
	std::unique_ptr<CWindow> Window;
	std::unique_ptr<IRenderSystem> RenderSystem;
	std::unique_ptr<CAssetManager> AssetManager;
	std::unique_ptr<CWorld> World;
	std::unique_ptr<CImGui> ImGui;
	std::atomic_bool GameLoop;
public:
	std::atomic_int GameThreadCounter;
	std::atomic_int RenderThreadCounter;
	/**
	 * Semaphore used to wait game thread
	 */
	CSemaphore GameThreadSemaphore;
public:
	glm::vec3 CameraPos;
	glm::vec3 CameraFront;
	glm::vec3 CameraUp;
};

extern CEngine* g_Engine;