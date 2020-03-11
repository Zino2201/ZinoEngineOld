#pragma once

#include "SceneRenderer.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/Renderer/Mesh/MeshPassEnum.h"
#include "Render/Renderer/Mesh/MeshPass.h"
#include "Render/UniformBuffer.h"

/**
 * Simple forward renderer
 * Inefficient with multiple lights
 */
class CForwardSceneRenderer : public ISceneRenderer
{
public:
    CForwardSceneRenderer();
    ~CForwardSceneRenderer();

    void Update();

    virtual void PreRender() override;
    virtual void Render(IRenderCommandContext* InCommandContext,
        CScene* InScene, const SViewport& InViewport) override;
private:
	void DrawWorld(IRenderCommandContext* InCommandContext,
		CScene* InScene);
    void ReloadImGui();
private:
    IRenderSystemUniformBufferPtr GlobalDataUBO;
    SSceneShaderGlobalData Data;
    class CImGuiRender* ImGuiRender;
    TUniformBuffer<SMeshPassViewData> ViewDataUBO;
};