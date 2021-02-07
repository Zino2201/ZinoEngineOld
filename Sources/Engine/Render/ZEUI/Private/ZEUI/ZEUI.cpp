#include "Module/Module.h"
#include "Gfx/Effect/EffectDatabase.h"

namespace ze::ui
{

class ZEUIModule : public module::Module
{
public:
	ZEUIModule()
	{
		gfx::effect_register_file("ZEUIBase",
			{
				{ gfx::ShaderStageFlagBits::Vertex, "Shaders/UI/ZEUIBaseVS.hlsl" },
				{ gfx::ShaderStageFlagBits::Fragment, "Shaders/UI/ZEUIBaseFS.hlsl" },
			},
			{});
	}
};

}

ZE_DEFINE_MODULE(ze::ui::ZEUIModule, ZEUI);