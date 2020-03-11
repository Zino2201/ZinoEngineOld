#include "ForwardSceneRenderer.h"
#include "Render/Commands/RenderCommandContext.h"
#include "Core/Engine.h"
#include "Core/RenderThread.h"
#include "Render/Renderer/Scene.h"
#include "Render/World/RenderableComponentProxy.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "UI/ImGuiRender.h"
#include "UI/imgui.h"
#include "Render/Window.h"
#include "Render/Renderer/Mesh/MeshPass.h"

DECLARE_COUNTER_STAT(Rendering, Drawcall);
DECLARE_TIMER_STAT(Rendering, ForwardSceneRenderTime);
DECLARE_TIMER_STAT(Rendering, GeometryPass);
DECLARE_TIMER_STAT(Rendering, UIPass);

std::array<float, 4> ClearColor = { 0.f, 0.f, 0.f, 1.0f };

CForwardSceneRenderer::CForwardSceneRenderer()
{
	ImGuiRender = new CImGuiRender;
	ImGuiRender->InitResources();

	ViewDataUBO.InitResources();

	g_Engine->GetRenderSystem()->GetFrameCompletedDelegate().Bind(
		std::bind(&CForwardSceneRenderer::ReloadImGui, this));
}

CForwardSceneRenderer::~CForwardSceneRenderer()
{
	ViewDataUBO.DestroyResources();
	ImGuiRender->DestroyResources();
	delete ImGuiRender;
}

void CForwardSceneRenderer::PreRender()
{
	
}

void CForwardSceneRenderer::ReloadImGui()
{
	ImGuiRender->UpdateBuffers();
}

void CForwardSceneRenderer::Render(IRenderCommandContext* InCommandContext,
	CScene* InScene, const SViewport& InViewport)
{
	SCOPED_TIMER_STAT(ForwardSceneRenderTime);

	SRect2D Scissor;
	Scissor.Position = {0, 0};
	Scissor.Size = { g_Engine->GetWindow()->GetWidth(),
		g_Engine->GetWindow()->GetHeight() };

	InCommandContext->SetScissors({ Scissor });

	/** Copy globals and bind */
	{
		SMeshPassViewData ViewData;
		glm::mat4 View = glm::lookAt(g_Engine->CameraPos, g_Engine->CameraPos 
			+ g_Engine->CameraFront,
			g_Engine->CameraUp);
		glm::mat4 Proj = glm::perspective(glm::radians(45.f),
			(float)g_Engine->GetWindow()->GetWidth() /
			(float)g_Engine->GetWindow()->GetHeight(), 0.1f, 10000.0f);
		Proj[1][1] *= -1;
		ViewData.ProjView = Proj * View;
		ViewDataUBO.Copy(&ViewData);
		
		InCommandContext->SetShaderUniformBuffer(0, 0,
			ViewDataUBO.GetUniformBuffer());
	}

	/** Set viewport */
	InCommandContext->SetViewports({ InViewport });

	/** Begin Geometry Pass */
	{
		SCOPED_TIMER_STAT(GeometryPass);
		InCommandContext->BeginRenderPass(ClearColor, "Geometry Pass");
		DrawWorld(InCommandContext, InScene);
		//InCommandContext->EndRenderPass();
	}

	/** Begin UI pass */
	{
		// TODO: Render pass handle
		SCOPED_TIMER_STAT(UIPass);
		//InCommandContext->BeginRenderPass(ClearColor, "UI Pass");
		ImGuiRender->Draw(InCommandContext);
		InCommandContext->EndRenderPass();
	}
}

void CForwardSceneRenderer::DrawWorld(IRenderCommandContext* InCommandContext,
	CScene* InScene)
{
	/** Draw with geometry pass shader */
	InScene->GetDrawCommandListManager().ExecuteCommands(InCommandContext, 
		EMeshPass::GeometryPass);
}