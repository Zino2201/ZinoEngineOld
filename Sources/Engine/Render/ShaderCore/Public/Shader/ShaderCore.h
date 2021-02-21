#pragma once

#include "EngineCore.h"
#include "Maths/MathCore.h"
#include "Module/Module.h"
#include "Serialization/Types/Map.h"
#include "Serialization/Types/Vector.h"
#include "Serialization/Types/String.h"
#include <robin_hood.h>

namespace ze::gfx::shaders
{

/**
 * Shaders
 */
enum class ShaderStage
{
    Vertex = 1 << 0,
    Fragment = 1 << 1,
};

enum class ShaderParameterType
{
	UniformBuffer,
	CombinedImageSampler,
	Texture,
	Sampler,
	StorageBuffer
};

struct ShaderParameterMember
{
	const char* name;
	uint64_t size;
	uint64_t offset;
};

/**
 * A shader parameter
 */
struct SHADERCORE_API ShaderParameter
{
	template<typename ArchiveType>
    void serialize(ArchiveType& archive)
    {
        archive <=> name;
        archive <=> type;
        archive <=> set;
        archive <=> binding;
        archive <=> size;
        archive <=> count;
        //archive <=> members;
    }

    std::string name;
    ShaderParameterType type;
    uint32_t set;
    uint32_t binding;
    uint64_t size;
    uint32_t count;
    std::vector<ShaderParameterMember> members;

	ShaderParameter() : type(ShaderParameterType::UniformBuffer),
		set(0), binding(0), size(0), count(0) {}

	ShaderParameter(const std::string& in_name,
		const ShaderParameterType& in_type,
		const uint32_t& in_set,
		const uint32_t& in_binding,
		const uint32_t& in_count) :
		name(in_name), type(in_type), set(in_set), binding(in_binding), size(0), count(in_count) {}

	ShaderParameter(const std::string& in_name,
		const ShaderParameterType& in_type,
		const uint32_t& in_set,
		const uint32_t& in_binding,
		const uint64_t& in_size,
		const uint32_t& in_count,
		const std::vector<ShaderParameterMember>& in_members) :
		name(in_name), type(in_type), set(in_set), binding(in_binding), size(in_size), count(in_count),
		members(in_members) {}
};

struct ShaderParameterHash
{
	std::size_t operator()(const ShaderParameter& parameter) const noexcept
	{
		std::size_t seed = 0;

		/**
		 * Don't hash set
		 */

		hash_combine(seed, parameter.name);
		hash_combine(seed, parameter.type);
		hash_combine(seed, parameter.binding);
		hash_combine(seed, parameter.size);
		hash_combine(seed, parameter.count);

		return seed;
	}
};

class ShaderParameterMap
{
public:
	template<typename ArchiveType>
    void serialize(ArchiveType& archive)
    {
        archive <=> parameters;
    }

	void add_parameter(const char* name, const ShaderParameter& parameter)
	{
		parameters.insert({ name, parameter});
	}

    const ShaderParameter& get_parameter_by_name(const std::string& name) const
	{
		return parameters.find(name)->second;
	}

	std::vector<ShaderParameter> get_parameters() const
	{
		std::vector<ShaderParameter> out_params;
		out_params.reserve(parameters.size());

		for(const auto& [name, param] : parameters)
		{
			out_params.push_back(param);
		}

		return out_params;
	}
private:
    robin_hood::unordered_map<std::string, ShaderParameter> parameters;
};

}