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
#include <SDL.h>
#include "Editor/Widgets/MapTabWidget.h"
#include "Assets/Asset.h"
#include "AssetDatabase/AssetDatabase.h"
#include <istream>
#include "Editor/Assets/AssetFactory.h"
#include "Assets/AssetManager.h"
#include "Editor/AssetUtils/AssetUtils.h"
#include "ZEFS/FileStream.h"

ZE_DEFINE_MODULE(ze::module::DefaultModule, ZEEditor);

namespace ze::editor
{

int StaticOnWindowResized(void* InUserData, SDL_Event* InEvent)
{
	ZE_ASSERT(InUserData);

	return reinterpret_cast<CZEEditor*>(InUserData)->OnWindowResized(InEvent);
}

SRSRenderPass MainRenderPass;

CZEEditor::CZEEditor() : CZinoEngineApp(true) 
{
	InitializeAssetFactoryMgr();
	assetutils::GetOnAssetImported().bind(
		std::bind(&CZEEditor::OnAssetImported, 
			this,
			std::placeholders::_1,
			std::placeholders::_2));

	/** Scan Assets directory */
	assetdatabase::scan("Assets", assetdatabase::AssetScanMode::Async);

	SDL_Rect WindowSize;
	SDL_GetDisplayUsableBounds(0, &WindowSize);

	MainWindow = std::make_unique<CWindow>("ZinoEngine Editor", WindowSize.w, WindowSize.h,
		EWindowFlagBits::Resizable | EWindowFlagBits::Centered | EWindowFlagBits::Maximized);
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
		Style.WindowRounding = 2.f;
		Style.TabRounding = 2.f;
	}

	IO.DisplaySize = ImVec2(static_cast<float>(MainWindow->GetWidth()), static_cast<float>(MainWindow->GetHeight()));
	IO.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	IO.WantCaptureKeyboard = true;
	IO.WantCaptureMouse = true;
	IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	
	Font = IO.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto-Medium.ttf", 16.f);

	renderer::CRendererModule::Get().CreateImGuiRenderer();

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

CZEEditor::~CZEEditor() 
{ 
	ClearAssetFactoryMgr();
	ImGui_ImplSDL2_Shutdown(); 
}

int CZEEditor::OnWindowResized(SDL_Event* InEvent)
{
	switch (InEvent->window.event)
	{
	case SDL_WINDOWEVENT_SIZE_CHANGED:
	case SDL_WINDOWEVENT_RESIZED:
		MainWindow->SetWidth(static_cast<uint32_t>(InEvent->window.data1));
		MainWindow->SetHeight(static_cast<uint32_t>(InEvent->window.data2));

		ImGuiIO& IO = ImGui::GetIO();
		IO.DisplaySize = ImVec2(MainWindow->GetWidth(), MainWindow->GetHeight());

		// Resize viewport
		MainViewport->Resize(MainWindow->GetWidth(), MainWindow->GetHeight());

		return SDL_TRUE;
	}

	return SDL_FALSE;
}

void CZEEditor::ProcessEvent(SDL_Event& InEvent)
{
	CZinoEngineApp::ProcessEvent(InEvent);

	ImGui_ImplSDL2_ProcessEvent(&InEvent);
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
	ImGui::SetNextWindowSize(ImVec2(static_cast<float>(MainWindow->GetWidth()), static_cast<float>(MainWindow->GetHeight())), 
		ImGuiCond_Once);
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
			ImGui::SameLine(ImGui::GetColumnWidth() - 500);
			ImGui::Text("ZinoEngine v%d.%d.%d (FPS: %f)",
				get_version().major, get_version().minor,
				get_version().patch, (1.f / InDeltaTime));
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

	renderer::CRendererModule::Get().GetImGuiRenderer()->CopyDrawdata();
	renderer::CRendererModule::Get().GetImGuiRenderer()->Update();

	if(MainViewport->Begin())
	{
		SRSFramebuffer Framebuffer;
		Framebuffer.ColorRTs[0] = MainViewport->GetSurface()->GetBackbufferTexture();
		GRSContext->BeginRenderPass(MainRenderPass, Framebuffer, { 1, 0, 0, 1 });
		GRSContext->SetViewports({ { { { 0, 0 }, { MainWindow->GetWidth(), MainWindow->GetHeight()} }, 0.f, 1.f } });
		GRSContext->SetScissors({ { { 0, 0 }, { MainWindow->GetWidth(), MainWindow->GetHeight()} } });
		renderer::CRendererModule::Get().GetImGuiRenderer()->Draw(GRSContext);
		GRSContext->EndRenderPass();
		MainViewport->End();
	}
}

void CZEEditor::OnAssetImported(const std::filesystem::path& InPath,
	const std::filesystem::path& InTarget)
{
#if 0
	/** Try to find an asset factory compatible with this format */
	std::string Extension = InPath.extension().string().substr(1, InPath.extension().string().size() - 1);
	CAssetFactory* Factory = GetFactoryForFormat(Extension);
	if (!Factory)
	{
		ze::logger::error("Asset {} can't be imported: unknown format", InPath.string());
		return;
	}

	/** Open a stream to the file */
	FileSystem::CIFileStream Stream(InPath, FileSystem::EFileReadFlagBits::Binary);
	if (!Stream)
	{
		ze::logger::error("Failed to open an input stream for file {}", InPath.string());
		return;
	}

	/** Serialize the asset and then unload it */
	CAsset* Asset = Factory->CreateFromStream(Stream);
	ZE::Editor::AssetUtils::SaveAsset(*Asset, InTarget,
		"NewAssetTest");
	delete Asset;

	/** Notify the database */
	ZE::AssetDatabase::Scan(InTarget);
#endif
}

OwnerPtr<CZinoEngineApp> CreateEditor()
{
	return new CZEEditor;
}

}