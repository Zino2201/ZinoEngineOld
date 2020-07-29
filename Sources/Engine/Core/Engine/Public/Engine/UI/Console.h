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
class ENGINE_API CConsoleWidget
{
public:
	CConsoleWidget();

	void Draw();
private:
	int OnTextEdited(ImGuiInputTextCallbackData* InData);
	ImVec4 ToColor(const Logger::ESeverityFlagBits& InSeverity) const;
private:
	std::array<char, 32> Input;
	size_t CurrentConsoleSize;
};

}