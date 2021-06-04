#include "imgui/ImGui.h"
#include "gfx/effect/EffectDatabase.h"

ZE_DEFINE_MODULE(ze::ui::ImGuiModule, imgui);

namespace ze::ui
{

ImGuiModule::ImGuiModule() 
{
	IMGUI_CHECKVERSION();
    gfx::effect_register_file("ImGui",
    {
    	{ gfx::ShaderStageFlagBits::Vertex, "Shaders/UI/ImGui.hlsl" },
    	{ gfx::ShaderStageFlagBits::Fragment, "Shaders/UI/ImGui.hlsl" },
    },{});
}

ImGuiModule::~ImGuiModule()
{
}

}