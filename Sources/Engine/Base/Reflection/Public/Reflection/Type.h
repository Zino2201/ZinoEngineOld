#pragma once

#include "EngineCore.h"
#include <cstdint>
#include "Traits.h"

namespace ze::reflection
{

class Class;

/**
 * Additional flags about a type
 */
enum class TypeFlagBits
{
	None = 0,

	Class = 1 << 0,
	Enum = 1 << 1,
	SequentialContainer = 1 << 2,
	AssociativeContainer = 1 << 3,
	Arithmetic = 1 << 4,
};
ENABLE_FLAG_ENUMS(TypeFlagBits, TypeFlags);

/**
 * Base class for basic types (e.g primitives)
 */
class Type
{
public:
	Type(const char* in_name,
		const size_t& in_size,
		const TypeFlags& in_flags) : name(in_name), size(in_size), flags(in_flags) {}

	Type(const Type&) = delete;
	void operator=(const Type&) = delete;

	/**
	 * Get a type by name, returns nullptr if not found
	 */
	REFLECTION_API static const Type* get_by_name(const std::string& in_name);
	
	/**
	 * Get a type by name, returns nullptr if not found
	 */
	template<typename T>
		requires IsReflType<T>
	ZE_FORCEINLINE static const Type* get()
	{
		return get_by_name(type_name<T>);
	}

	ZE_FORCEINLINE const std::string& get_name() const { return name; }
	ZE_FORCEINLINE const size_t& get_size() const { return size; }
	ZE_FORCEINLINE bool is_arithmetic() const { return static_cast<bool>(flags & TypeFlagBits::Arithmetic); }
	ZE_FORCEINLINE bool is_class() const { return static_cast<bool>(flags & TypeFlagBits::Class); }
	ZE_FORCEINLINE bool is_enum() const { return static_cast<bool>(flags & TypeFlagBits::Enum); }
	ZE_FORCEINLINE bool is_sequential_container() const { return static_cast<bool>(flags & TypeFlagBits::SequentialContainer); }
	ZE_FORCEINLINE bool is_associative_container() const { return static_cast<bool>(flags & TypeFlagBits::AssociativeContainer); }
private:
	std::string name;
	size_t size;
	TypeFlags flags;
};

/**
 * A "lazy" type pointer used in reflection structures
 * to point to a type that may not be registered as the registration
 * order is undefined
 */
class LazyTypePtr
{
public:
	LazyTypePtr() : type(nullptr) {} 
	LazyTypePtr(const std::string& in_name) : name(in_name), type(nullptr) { get(); }

	const Type* get() const
	{
		if(!type)
			type = Type::get_by_name(name);

		return type;
	}

	const Class* get_as_class() const
	{
		if(get() && type->is_class())
			return reinterpret_cast<const Class*>(type);

		return nullptr;
	}
private:
	std::string name;
	mutable const Type* type;
};

}