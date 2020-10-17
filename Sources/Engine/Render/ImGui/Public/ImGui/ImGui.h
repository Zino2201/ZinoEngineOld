#pragma once

#include "EngineCore.h"
#include <imgui.h>
#include "Module/Module.h"

namespace ze::ui
{

class IMGUI_API ImGuiModule : public ze::module::Module
{
public:
	ImGuiModule();
	~ImGuiModule();
};

struct SImGuiAutoStyleColor
{
	SImGuiAutoStyleColor(ImGuiCol InIdx, const ImVec4& InColor)
	{
		ImGui::PushStyleColor(InIdx, InColor);
	}

	~SImGuiAutoStyleColor()
	{
		ImGui::PopStyleColor();
	}
};

struct SImGuiAutoStyleVar
{
	SImGuiAutoStyleVar(ImGuiStyleVar InIdx, const ImVec2& InVal)
	{
		ImGui::PushStyleVar(InIdx, InVal);
	}

	~SImGuiAutoStyleVar()
	{
		ImGui::PopStyleVar();
	}
};
}