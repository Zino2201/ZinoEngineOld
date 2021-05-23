#pragma once

#include "assets/Asset.h"
#include "Effect.gen.h"

namespace ze
{

/**
 * Type of the effect
 */
ZENUM()
enum class EffectType
{
	/** Surface effect: will use default engine PBR lighting techniques */
	Surface,

	/** Custom effect: useful for post-processing effects or other */
	Custom
};

/**
 * An effect, this is the base object of the rendering pipeline
 * (wraps the ze::gfx::Effect type)
 */
ZCLASS()
class Effect : public Asset
{
	ZE_REFL_BODY()

	friend class EffectCooker;

public:
	enum EffectAssetFormat
	{
		FormatSource,
		FormatCompiled
	};

	std::string& get_source() { return source; }
private:
	ZPROPERTY(Editable, Visible, Category = "Base")
	EffectType type;

	std::string source;
};

}