#include "Editor/Widgets/ConvarViewer.h"
#include "ImGui/ImGui.h"
#include "Console/Console.h"

namespace ze::editor
{

void ConvarViewer::draw()
{
	if(ImGui::Begin("Convar Viewer"))
	{
		const float FooterHeight =
			ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

		if (ImGui::BeginChild("ScrollingRegion",
			ImVec2(0, -FooterHeight), true, ImGuiWindowFlags_HorizontalScrollbar))
		{
			for(auto& convar : CConsole::Get().get_convars())
			{
				ImGui::TextUnformatted(convar.name.c_str());
				if(ImGui::IsItemHovered())
					ImGui::SetTooltip("%s", convar.help.c_str());
				ImGui::SameLine();
				if(convar.data.index() == ConVar::DataTypeFloat)
					ImGui::SliderFloat(std::string("##" + convar.name).c_str(), &std::get<float>(convar.data),
						convar.get_min_as_float(), convar.get_max_as_float());
				else if (convar.data.index() == ConVar::DataTypeInt32)
					ImGui::SliderInt(std::string("##" + convar.name).c_str(), &std::get<int32_t>(convar.data),
						convar.get_min_as_int(), convar.get_max_as_int());
				//else
				//	ImGui::InputText("", &std::get<int32_t>(convar.data));
				ImGui::SameLine();
				if(ImGui::ArrowButton(std::string("##BTN" + convar.name).c_str(), ImGuiDir_Left))
					convar.set_data(convar.default_value);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Reset value");
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

}