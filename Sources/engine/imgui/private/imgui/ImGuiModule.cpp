#include "imgui/ImGui.h"

ZE_DEFINE_MODULE(ze::ui::ImGuiModule, ImGui);

namespace ze::ui
{

ImGuiModule::ImGuiModule() 
{
	IMGUI_CHECKVERSION();
}

ImGuiModule::~ImGuiModule()
{
}

}