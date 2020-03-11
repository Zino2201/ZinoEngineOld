#pragma once

#include "EngineCore.h"

class CWindow;
class IRenderSystem;
class CAssetManager;
class CWorld;
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
	CTimerManager& GetTimerManager() { return TimerManager; }
private:
	void Loop();
public:
	CEngine();
	~CEngine();
private:
	bool bHasBeenInitialized;
	std::unique_ptr<CWindow> Window;
	std::unique_ptr<IRenderSystem> RenderSystem;
	std::unique_ptr<CAssetManager> AssetManager;
	std::unique_ptr<CWorld> World;
	std::atomic_bool GameLoop;
	CTimerManager TimerManager;
public:
	std::atomic_int GameThreadCounter;
	std::atomic_int RenderThreadCounter;
	/**
	 * Semaphore used to wait game thread
	 */
	CSemaphore GameThreadSemaphore;
	float DeltaTime;
	float ElapsedTime;
public:
	glm::vec3 CameraPos;
	glm::vec3 CameraFront;
	glm::vec3 CameraUp;
};

extern CEngine* g_Engine;

FORCEINLINE CTimerManager& GetTimerManager()
{
	return g_Engine->GetTimerManager();
}