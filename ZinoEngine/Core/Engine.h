#pragma once

#include "EngineCore.h"

class CWindow;
class IRenderSystem;
class IRenderer;
class CAssetManager;
class CWorld;
class CImGui;
class CRenderCommandList;
class CSceneProxy;

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
	IRenderer* GetRenderer() const { return Renderer.get(); }
	CAssetManager* GetAssetManager() const { return AssetManager.get(); }
	CSceneProxy* GetSceneProxy() const { return SceneProxy.get(); }
private:
	void Loop();
	void InitImGui();

	/**
	 * Only used for creating the render thread at the end of the first game thread frame
	 */
	void DrawScene();

	/**
	 * Render thread main
	 */
	void RenderThreadMain();
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
	std::unique_ptr<IRenderer> Renderer;
	std::unique_ptr<CAssetManager> AssetManager;
	std::unique_ptr<CWorld> World;
	std::unique_ptr<CImGui> ImGui;
	std::thread RenderThread;
	std::atomic_bool Run;
	std::unique_ptr<CRenderCommandList> MainCommandList;
	std::unique_ptr<CSceneProxy> SceneProxy;
};