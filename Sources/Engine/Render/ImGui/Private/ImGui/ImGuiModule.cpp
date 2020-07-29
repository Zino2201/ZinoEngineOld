#include "ImGui/ImGui.h"

DEFINE_MODULE(ZE::UI::CImGuiModule, ImGui);

namespace ZE::UI
{

CImGuiModule::CImGuiModule() 
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
}

CImGuiModule::~CImGuiModule()
{
	ImGui::DestroyContext();
}

}