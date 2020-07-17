#pragma once

#include <imgui.h>
#include "Module/Module.h"

namespace ZE::UI
{

class IMGUI_API CImGuiModule : public CModule
{
public:
	void Initialize() override;
	void Destroy() override;
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