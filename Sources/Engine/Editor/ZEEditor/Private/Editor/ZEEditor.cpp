#include "Editor/ZEEditor.h"
#include "Module/Module.h"
#include "Render/Window.h"
#include "Engine/Viewport.h"
#include "ImGui/ImGui.h"
#include "examples/imgui_impl_sdl.h"
#include "Renderer/RendererModule.h"
#include "Render/RenderSystem/RenderSystemContext.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/RenderSystem/Resources/RenderPass.h"
#include "Render/RenderSystem/Resources/Framebuffer.h"
#include "Render/RenderSystem/Resources/Pipeline.h"
#include "Render/RenderSystem/Resources/Surface.h"
#include "ImGui/ImGuiRender.h"
#include <SDL2/SDL.h>
#include "Editor/Widgets/MapTabWidget.h"

DEFINE_MODULE(ZE::CModule, Editor);

namespace ZE::Editor
{

CZEEditor::~CZEEditor() = default;

SRSRenderPass MainRenderPass;

int StaticOnWindowResized(void* InUserData, SDL_Event* InEvent)
{
	must(InUserData);

	return reinterpret_cast<CZEEditor*>(InUserData)->OnWindowResized(InEvent);
}

void CZEEditor::Initialize()
{
	bShouldRun = true;

	SDL_Rect WindowSize;
	SDL_GetDisplayUsableBounds(0, &WindowSize);

	MainWindow = std::make_unique<CWindow>("ZinoEngine Editor", WindowSize.w, WindowSize.h,
		EWindowFlags::Borderless | EWindowFlags::Resizable);
	MainViewport = std::make_unique<CViewport>(MainWindow->GetHandle(), MainWindow->GetWidth(),
		MainWindow->GetHeight(), true);

	/** Event on resize */
	SDL_AddEventWatch(&StaticOnWindowResized, this);

	ImGui_ImplSDL2_InitForVulkan(reinterpret_cast<SDL_Window*>(MainWindow->GetHandle()));

	ImGuiIO& IO = ImGui::GetIO();
	ImGui::StyleColorsDark();
	/** Default ZE editor style */
	{
		ImGuiStyle& Style = ImGui::GetStyle();
		Style.WindowRounding = 0.f;
		Style.TabRounding = 0.f;
	}

	IO.DisplaySize = ImVec2(static_cast<float>(MainWindow->GetWidth()), static_cast<float>(MainWindow->GetHeight()));
	IO.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	IO.WantCaptureKeyboard = true;
	IO.WantCaptureMouse = true;
	IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	Font = IO.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto-Medium.ttf", 16.f);

	Renderer::CRendererModule::Get().CreateImGuiRenderer();

	MainRenderPass =
	{
		/** Colors */
		{
			/** Main viewport */
			{
				{
					MainViewport->GetSurface()->GetSwapChainFormat(),
					ESampleCount::Sample1,
					ERSRenderPassAttachmentLoadOp::DontCare,
					ERSRenderPassAttachmentStoreOp::Store,
					ERSRenderPassAttachmentLayout::Undefined,
					ERSRenderPassAttachmentLayout::Present,
				}
			}
		},
		/** Depths */
		{},
		/** Subpasses */
		{
			{
				/** Main viewport */
				{
					/** Color */
					{
						0,
						ERSRenderPassAttachmentLayout::ColorAttachment
					}
				},
				{},
				{}
			},
		}
	};
}


int CZEEditor::OnWindowResized(SDL_Event* InEvent)
{
	switch (InEvent->window.event)
	{
	case SDL_WINDOWEVENT_SIZE_CHANGED:
	case SDL_WINDOWEVENT_RESIZED:
		MainWindow->SetWidth(static_cast<uint32_t>(InEvent->window.data1));
		MainWindow->SetHeight(static_cast<uint32_t>(InEvent->window.data2));

		// Resize viewport
		MainViewport->Resize(MainWindow->GetWidth(), MainWindow->GetHeight());

		return SDL_TRUE;
	}

	return SDL_FALSE;
}

void CZEEditor::ProcessEvent(SDL_Event* InEvent)
{
	ImGui_ImplSDL2_ProcessEvent(InEvent);
}

void CZEEditor::DrawMainTab()
{
	/** Main tab */
	if (!ImGui::BeginTabItem("Main"))
		return;

	static CMapTabWidget Wid;
	Wid.Draw();

	ImGui::EndTabItem();
}

void CZEEditor::Tick(const float& InDeltaTime)
{
	ImGui_ImplSDL2_NewFrame(reinterpret_cast<SDL_Window*>(MainWindow->GetHandle()));
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(static_cast<float>(MainWindow->GetWidth()), static_cast<float>(MainWindow->GetHeight())));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if(ImGui::Begin("MainEditorWindow", nullptr, ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus))
	{
		ImGui::PopStyleVar(3);

		if (ImGui::BeginTabBar("MainTabBar", ImGuiTabBarFlags_TabListPopupButton
			| ImGuiTabBarFlags_Reorderable))
		{
			ImGui::SameLine(ImGui::GetColumnWidth() - 200);
			ImGui::Text("ZinoEngine");
			ImGui::SameLine(ImGui::GetColumnWidth() - 40);
			if (ImGui::Button("X"))
				bShouldRun = false;
			DrawMainTab();
			ImGui::EndTabBar();
		}
	}

	ImGui::End();
	ImGui::Render();
}

void CZEEditor::Draw()
{
	/** Trigger rendering */
	GRenderSystem->NewFrame();

	Renderer::CRendererModule::Get().GetImGuiRenderer()->CopyDrawdata();
	Renderer::CRendererModule::Get().GetImGuiRenderer()->Update();

	if(MainViewport->Begin())
	{
		SRSFramebuffer Framebuffer;
		Framebuffer.ColorRTs[0] = MainViewport->GetSurface()->GetBackbufferTexture();
		GRSContext->BeginRenderPass(MainRenderPass, Framebuffer, { 1, 0, 0, 1 });
		GRSContext->SetViewports({ { { { 0, 0 }, { MainWindow->GetWidth(), MainWindow->GetHeight()} }, 0.f, 1.f } });
		GRSContext->SetScissors({ { { 0, 0 }, { MainWindow->GetWidth(), MainWindow->GetHeight()} } });
		Renderer::CRendererModule::Get().GetImGuiRenderer()->Draw(GRSContext);
		GRSContext->EndRenderPass();
		MainViewport->End();
	}
}

void CZEEditor::Exit()
{
	ImGui_ImplSDL2_Shutdown();
}

TOwnerPtr<CEngine> CreateEditor()
{
	return new CZEEditor;
}

}