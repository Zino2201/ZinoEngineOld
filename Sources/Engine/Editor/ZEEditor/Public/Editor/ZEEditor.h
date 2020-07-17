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

class CZEEditor : public CEngine
{
public:
	~CZEEditor();

	void Initialize() override;
	void ProcessEvent(SDL_Event* InEvent) override;
	void Tick(const float& InDeltaTime) override;
	void Draw() override;
	void Exit() override;
	int OnWindowResized(SDL_Event* InEvent);
	bool ShouldExit() const override { return !bShouldRun; }
private:
	void DrawMainTab();
private:
	std::unique_ptr<CWindow> MainWindow;
	std::unique_ptr<CViewport> MainViewport;
	ImFont* Font;
	bool bShouldRun;
};

TOwnerPtr<CEngine> CreateEditor();

}