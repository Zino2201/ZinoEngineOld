#include "ZEUI/ZEUI.h"
#include "Gfx/Effect/EffectDatabase.h"

namespace ze::ui
{

ZEUIModule::ZEUIModule()
{
	gfx::effect_register_file("ZEUIBase",
	{
		{ gfx::ShaderStageFlagBits::Vertex, "Shaders/UI/ZEUIBaseVS.hlsl" },
		{ gfx::ShaderStageFlagBits::Fragment, "Shaders/UI/ZEUIBaseFS.hlsl" },
		},
	{});

	gfx::effect_register_file("ZEUIDistanceFieldText",
	{
		{ gfx::ShaderStageFlagBits::Vertex, "Shaders/UI/ZEUIDistanceFieldTextVS.hlsl" },
		{ gfx::ShaderStageFlagBits::Fragment, "Shaders/UI/ZEUIDistanceFieldTextFS.hlsl" },
		},
	{});
}

}

ZE_DEFINE_MODULE(ze::ui::ZEUIModule, ZEUI);