#include "Module/Module.h"
#include "Gfx/Effect/EffectDatabase.h"

namespace ze::gfx
{

class EffectSystemModule : public module::Module
{
public:
	~EffectSystemModule()
	{
		effect_destroy_all();
	}
};

}

ZE_DEFINE_MODULE(ze::gfx::EffectSystemModule, EffectSystem);