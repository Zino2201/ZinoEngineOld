#pragma once

#include "Engine/Engine.h"

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
protected:
	void Draw() override;
private:
	void DrawMainTab();
private:
	std::unique_ptr<CWindow> MainWindow;
	std::unique_ptr<CViewport> MainViewport;
	ImFont* Font;
	bool bShouldRun;
};

ZEEDITOR_API TOwnerPtr<CZinoEngineApp> CreateEditor();

}