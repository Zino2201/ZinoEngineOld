#include "ImGui/ImGui.h"

DEFINE_MODULE(ZE::UI::CImGuiModule, ImGui);

namespace ZE::UI
{

void CImGuiModule::Initialize()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
}

void CImGuiModule::Destroy()
{
	ImGui::DestroyContext();
}

}