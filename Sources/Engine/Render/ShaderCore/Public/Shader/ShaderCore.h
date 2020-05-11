#pragma once

#include "EngineCore.h"

/**
 * Shaders
 */
enum class EShaderStage
{
    Vertex = 1 << 0,
    Fragment = 1 << 1,
};

enum class EShaderParameterType
{
	UniformBuffer,
	CombinedImageSampler,
	StorageBuffer
};

struct SShaderParameterMember
{
	const char* Name;
	uint64_t Size;
	uint64_t Offset;
};

/**
 * A shader parameter
 */
struct SShaderParameter
{
    const char* Name;
    EShaderParameterType Type;
    uint32_t Set;
    uint32_t Binding;
    uint64_t Size;
    uint32_t Count;
    std::vector<SShaderParameterMember> Members;
};

struct SShaderParameterHash
{
	std::size_t operator()(const SShaderParameter& InParameter) const noexcept
	{
		std::size_t Seed = 0;

		/**
		 * Don't hash set
		 */

		HashCombine(Seed, InParameter.Name);
		HashCombine(Seed, InParameter.Type);
		HashCombine(Seed, InParameter.Binding);
		HashCombine(Seed, InParameter.Size);
		HashCombine(Seed, InParameter.Count);

		return Seed;
	}
};

class SShaderParameterMap
{
public:
	void AddParameter(const char* InName, const SShaderParameter& InParameter)
	{
		Parameters.insert(std::make_pair(InName, InParameter));
	}

    const SShaderParameter& GetParameterByName(const CString& InName) const
	{
		return Parameters.find(InName)->second;
	}

	std::vector<SShaderParameter> GetParameters() const
	{
		std::vector<SShaderParameter> OutParameters;
		OutParameters.reserve(Parameters.size());

		for(const auto& [Name, Parameter]: Parameters)
		{
			OutParameters.push_back(Parameter);
		}

		return OutParameters;
	}
private:
    std::unordered_map<CString, SShaderParameter> Parameters;
};