#pragma once

#include "EngineCore.h"
#include <array>

struct ImGuiInputTextCallbackData;
struct ImVec4;

namespace ZE
{

/**
 * Console widget
 */
class CConsoleWidget
{
public:
	void Draw();
private:
	int OnTextEdited(ImGuiInputTextCallbackData* InData);
	ImVec4 ToColor(const ELogSeverity& InSeverity) const;
private:
	std::array<char, 32> Input;
};

}