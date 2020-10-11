#pragma once

#include "Engine/Engine.h"
#include <filesystem>

struct ImFont;

namespace ZE 
{ 
class CWindow; 
class CViewport; 
}

namespace ZE::Editor
{

class CZEEditor final : public CZinoEngineApp
{
public:
	CZEEditor();
	~CZEEditor();

	void ProcessEvent(SDL_Event& InEvent) override;
	void Tick(const float& InDeltaTime) override;
	int OnWindowResized(SDL_Event* InEvent);
	
	void OnAssetImported(const std::filesystem::path& InPath,
		const std::filesystem::path& InTarget);
protected:
	void Draw() override;
private:
	void DrawMainTab();
private:
	std::unique_ptr<CWindow> MainWindow;
	std::unique_ptr<CViewport> MainViewport;
	ImFont* Font;
};

ZEEDITOR_API TOwnerPtr<CZinoEngineApp> CreateEditor();

}