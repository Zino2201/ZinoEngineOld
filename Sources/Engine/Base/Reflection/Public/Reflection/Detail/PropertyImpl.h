#pragma once

#include "EngineCore.h"
#include "Reflection/Any.h"

namespace ze::reflection::detail
{

/**
 * Base property implementation class
 */
class REFLECTION_API PropertyImplBase
{
public:
	PropertyImplBase(const size_t& in_offset) : offset(in_offset) {}
	virtual ~PropertyImplBase() = default;

	virtual Any get_value(const void* instance) const { return {}; }
	virtual void set_value(const void* instance, const std::any& value) const {}
protected:
	size_t offset;
};

/**
 * Member property implementation
 */
template<typename T>
class PropertyImplMember : public PropertyImplBase
{
public:
	PropertyImplMember(const size_t& in_offset) : PropertyImplBase(in_offset) {}

	Any get_value(const void* instance) const override
	{
		return Any(reinterpret_cast<T&>(*reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(instance) + offset)));
	}

	void set_value(const void* instance, const std::any& value) const override
	{
		reinterpret_cast<T&>(*reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(instance) + offset)) = std::any_cast<T>(value);
	}
};

}