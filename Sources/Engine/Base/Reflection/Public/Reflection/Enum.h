#pragma once

#include "Type.h"
#include "Any.h"

namespace ze::reflection
{

class Enum : public Type
{
public:
	Enum(const char* in_name,
		const size_t& in_size,
		const TypeFlags& in_flags) : Type(in_name, in_size, in_flags) {}

	REFLECTION_API static const Enum* get_by_name(const std::string in_name);
	
	template<typename T>
	ZE_FORCEINLINE static const Enum* get()
	{
		return get_by_name(type_name<T>);
	}

	REFLECTION_API std::string get_value_name(const Any& in_value) const;

	const LazyTypePtr& get_underlying_type_lazy_ptr() const { return underlying_type; }
	const auto& get_values() const { return values; }
private:
	std::vector<std::pair<std::string, Any>> values;
	LazyTypePtr underlying_type;
};

}