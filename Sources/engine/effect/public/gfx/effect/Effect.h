#pragma once

#include "EngineCore.h"
#include "gfx/Gfx.h"
#include <robin_hood.h>
#include <bitset>
#if ZE_WITH_EDITOR
#include "gfx/effect/EffectCompiler.h"
#endif

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

enum class EffectPermutationStatus
{
	/** Permutation is not present or have not been built */
	Unavailable,

	/** Permutation have a compile error and can't be used. It can be recompiled if possible */
	CompileError,

	/** Permutation is available !*/
	Available
};
	
using EffectShaderSources = robin_hood::unordered_map<ShaderStageFlagBits, std::string>;
using EffectShaderMap = robin_hood::unordered_map<ShaderStageFlagBits, DeviceResourceHandle>;
using EffectPermutationId = std::bitset<32>;
	
/*
 * A effect: a group of shaders and metadatas describing the state of the final graphics pipeline for a single pass
 * This works similary like D3D Effect API/Unity ShaderLab language
 * An effect can have options, that generate multiple permutations for supporting multiple features
 */
class Effect
{
	static constexpr size_t max_permutation_bits = 32;

public:
	/**
	 * A single permutation
	 */
	struct Permutation
	{
		EffectPermutationStatus status;
		EffectShaderMap shader_map;
		UniquePipelineLayout pipeline_layout;

		Permutation() : status(EffectPermutationStatus::Unavailable) {}
	};

	Effect(const std::string& in_name,
		const EffectShaderSources& in_sources,
		const std::vector<EffectOption>& in_options);
	~Effect();

	Effect(const Effect&) = delete;
	void operator=(const Effect&) = delete;

#if ZE_WITH_EDITOR
	/**
	 * Compile the effect (async)
	 */
	std::future<EffectCompilerResult> compile(const EffectPermutationId id, const ShaderFormat& in_format);
#endif
	
	/**
	 * Get the shader map of the specific permutation
	 * If the permutation is not build, it will be built
	 * \param id Id of the permutation
	 */
	Permutation* get_permutation(const EffectPermutationId id);
	EffectPermutationId get_permutation_id(const std::vector<std::pair<std::string, uint32_t>>& in_enabled_options) const;
	std::vector<std::pair<std::string, std::string>> get_options_from_id(EffectPermutationId id) const;
	bool is_available(const EffectPermutationId id);
	ZE_FORCEINLINE const PipelineRasterizationStateCreateInfo& get_rasterizer_state() const { return rasterizer_state; }
private:
	void destroy_permutation(EffectPermutationId id);
private:
	std::string name;
	EffectShaderSources sources;
	robin_hood::unordered_map<std::string, size_t> name_to_option_idx;
	std::vector<EffectOption> options;
	size_t permutation_count;
	robin_hood::unordered_map<EffectPermutationId, Permutation> permutations;
#if ZE_WITH_EDITOR
	robin_hood::unordered_set<EffectPermutationId> pending_compilation;
#endif
	PipelineRasterizationStateCreateInfo rasterizer_state;
};

/**
 * Pointer to a specific permutation in a effect
 */
struct EffectPermPtr
{
	Effect* effect;
	EffectPermutationId permutation;

	EffectPermPtr(std::nullptr_t) : effect(nullptr) {}
	EffectPermPtr(Effect* in_effect, const EffectPermutationId& in_perm) : effect(in_effect),
		permutation(in_perm) {}

	Effect::Permutation* operator->() const
	{
		ZE_CHECK(effect);
		auto perm = effect->get_permutation(permutation);

		// TODO: Better way to wait... i'm too lazy to do it now
		while(!perm || !effect->is_available(permutation))
		{
			perm = effect->get_permutation(permutation);
		}
		return perm;
	}

	bool operator==(const EffectPermPtr& other) const
	{
		return effect == other.effect && permutation == other.permutation;
	}

	bool operator!=(const EffectPermPtr& other) const
	{
		return effect != other.effect || permutation != other.permutation;
	}

	operator bool() const
	{
		return effect;
	}
};

}