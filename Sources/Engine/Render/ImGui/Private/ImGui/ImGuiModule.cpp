#include "ImGui/ImGui.h"

ZE_DEFINE_MODULE(ze::ui::ImGuiModule, ImGui);

namespace ze::ui
{

ImGuiModule::ImGuiModule() 
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
}

ImGuiModule::~ImGuiModule()
{
	ImGui::DestroyContext();
}

}