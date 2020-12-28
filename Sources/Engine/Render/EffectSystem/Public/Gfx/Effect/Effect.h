#pragma once

#include "EngineCore.h"
#include "Gfx/Backend.h"
#include <robin_hood.h>
#include <bitset>

namespace ze::gfx
{

enum class EffectOptionType
{
	Bool,
	Int,
};

struct EffectOption
{
	std::string name;
	EffectOptionType type;
	uint32_t count;
	size_t bit_width;

	/** Index of the option in a permutation ID */
	size_t id_index;

	EffectOption(const std::string& in_name,
		const EffectOptionType& in_type,
		const int32_t& in_count = -1) : name(in_name), type(in_type), bit_width(0), id_index(0)
	{
		switch(type)
		{
		case EffectOptionType::Bool:
			count = 1;
			break;
		case EffectOptionType::Int:
			count = in_count;
			break;
		}
	}
};

using EffectShaderSources = robin_hood::unordered_map<ShaderStageFlagBits, std::string>;
using EffectShaderMap = robin_hood::unordered_map<ShaderStageFlagBits, ResourceHandle>;
using EffectPermutationId = std::bitset<32>;

/*
 * A effect: a group of shaders and metadatas describing the state of the final graphics pipeline for a single pass
 * This works similary like D3D Effect API/Unity ShaderLab language
 * An effect can have options, that generate multiple permutations for supporting multiple features
 */
class EFFECTSYSTEM_API Effect
{
	static constexpr size_t max_permutation_bits = 32;

public:
	Effect(const std::string& in_name,
		const EffectShaderSources& in_sources,
		const std::vector<EffectOption>& in_options);
	~Effect();

	/**
	 * Get the shader map of the specific permutation
	 * If the permutation is not build, it will be built
	 */
	const EffectShaderMap get_permutation(EffectPermutationId id);
	EffectPermutationId get_permutation_id(const std::vector<std::pair<std::string, uint32_t>>& in_enabled_options) const;
	std::vector<std::pair<std::string, std::string>> get_options_from_id(EffectPermutationId id) const;
private:
#if ZE_WITH_EDITOR
	void compile_permutation(EffectPermutationId id);
#endif
	void destroy_permutation(EffectPermutationId id);
private:
	std::string name;
	EffectShaderSources sources;
	robin_hood::unordered_map<std::string, size_t> name_to_option_idx;
	std::vector<EffectOption> options;
	size_t permutation_count;
	robin_hood::unordered_map<EffectPermutationId, EffectShaderMap> permutations;
#if ZE_WITH_EDITOR
	robin_hood::unordered_set<EffectPermutationId> pending_compilation;
#endif
};

}