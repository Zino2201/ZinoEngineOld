#pragma once

#include "Effect.h"

namespace ze::gfx
{

/** Effect database */
void effect_register(const std::string& in_name,
	const EffectShaderSources& in_sources,
	const std::vector<EffectOption>& in_options);

/**
 * Register a new effect from files
 */
void effect_register_file(const std::string& in_name,
	const EffectShaderSources& in_sources,
	const std::vector<EffectOption>& in_options);

void effect_destroy_all();

Effect* effect_get_by_name(const std::string& in_name);

}