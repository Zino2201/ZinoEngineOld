#pragma once

#include "Type.h"
#include "Any.h"

namespace ze::reflection
{

class REFLECTION_API Enum : public Type
{
public:
	Enum(const char* in_name,
		const size_t& in_size,
		const TypeFlags& in_flags) : Type(in_name, in_size, in_flags) {}

	static const Enum* get_by_name(const std::string& in_name);
	
	template<typename T>
	ZE_FORCEINLINE static const Enum* get()
	{
		return get_by_name(type_name<T>);
	}

	std::string get_value_name(const Any& in_value) const;

	const LazyTypePtr& get_underlying_type_lazy_ptr() const { return underlying_type; }
	const auto& get_values() const { return values; }
private:
	std::vector<std::pair<std::string, Any>> values;
	LazyTypePtr underlying_type;
};

}

namespace std
{

template<typename T>
	requires ze::reflection::IsReflEnum<T>
std::string to_string(const T& in_value)
{
	return static_cast<const ze::reflection::Enum*>(ze::reflection::Type::get<T>())->get_value_name(in_value);
}

}