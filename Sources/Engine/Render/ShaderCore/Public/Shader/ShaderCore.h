#pragma once

#include "EngineCore.h"
#include "Module/Module.h"

namespace ZE
{

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
	Texture,
	Sampler,
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
struct SHADERCORE_API SShaderParameter
{
    std::string Name;
    EShaderParameterType Type;
    uint32_t Set;
    uint32_t Binding;
    uint64_t Size;
    uint32_t Count;
    std::vector<SShaderParameterMember> Members;

	SShaderParameter() : Name(), Type(EShaderParameterType::UniformBuffer),
		Set(0), Binding(0), Size(0), Count(0) {}

	SShaderParameter(const std::string& InName,
		const EShaderParameterType& InType,
		const uint32_t& InSet,
		const uint32_t& InBinding,
		const uint32_t& InCount) :
		Name(InName), Type(InType), Set(InSet), Binding(InBinding), Size(0), Count(InCount) {}

	SShaderParameter(const std::string& InName,
		const EShaderParameterType& InType,
		const uint32_t& InSet,
		const uint32_t& InBinding,
		const uint64_t& InSize,
		const uint32_t& InCount,
		const std::vector<SShaderParameterMember>& InMembers) :
		Name(InName), Type(InType), Set(InSet), Binding(InBinding), Size(InSize), Count(InCount),
		Members(InMembers) {}
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

}