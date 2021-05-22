/** #zert skip */

#pragma once

/**
 * Macros used for the reflection system
 */

#include "Traits.h"
#include "Class.h"

#define ZE_REFL_DECLARE_TYPE(Type) \
	template<> static constexpr bool ze::reflection::is_refl_type<Type> = true; \
	template<> static constexpr const char* ze::reflection::type_name<Type> = #Type;

#define ZE_REFL_DECLARE_CLASS(InClass) \
	ZE_REFL_DECLARE_TYPE(InClass) \
	template<> static constexpr bool ze::reflection::is_refl_class<InClass> = true; \
	template<> static constexpr bool ze::reflection::serialization::is_serializable_with_reflection<InClass> = true;
	
#define ZE_REFL_DECLARE_STRUCT_BODY(InStruct) \
	friend void ze::reflection::initialize_reflection_data(); \

#define ZE_REFL_DECLARE_CLASS_BODY(InClass) \
	public: \
	friend void ze::reflection::initialize_reflection_data(); \
	virtual const ze::reflection::Class* get_class() const { return ze::reflection::Class::get<InClass>(); } \
	private:

#define ZE_REFL_DECLARE_CLASS_BODY_WITH_OVERRIDE(InClass) \
	public: \
	friend void ze::reflection::initialize_reflection_data(); \
	virtual const ze::reflection::Class* get_class() const override { return ze::reflection::Class::get<InClass>(); } \
	private:

#define ZE_REFL_DECLARE_ENUM(InEnum) \
	ZE_REFL_DECLARE_TYPE(InEnum) \
	template<> static constexpr bool ze::reflection::is_refl_enum<InEnum> = true;

/**
 * Macros parsed by the reflection tool
 */
#define ZSTRUCT(...)
#define ZCLASS(...)
#define ZENUM(...)
#define ZPROPERTY(...)
#define ZFUNCTION(...)

#if __INTELLISENSE__ || __JETBRAINS_IDE__
#define ZE_REFL_BODY()
#else
/** Macro used for ZRT to generate a body */
#define ZE_REFL_BODY() ZE_CONCAT(ZE_CONCAT(ZE_CONCAT(ZE_Refl_Body_, ZE_CURRENT_FILE_UNIQUE_ID), _), __LINE__)
#endif

ZE_REFL_DECLARE_TYPE(bool)
ZE_REFL_DECLARE_TYPE(float)
ZE_REFL_DECLARE_TYPE(double)

ZE_REFL_DECLARE_TYPE(uint8_t)
ZE_REFL_DECLARE_TYPE(uint16_t)
ZE_REFL_DECLARE_TYPE(uint32_t)
ZE_REFL_DECLARE_TYPE(uint64_t)

ZE_REFL_DECLARE_TYPE(int8_t)
ZE_REFL_DECLARE_TYPE(int16_t)
ZE_REFL_DECLARE_TYPE(int32_t)
ZE_REFL_DECLARE_TYPE(int64_t)

template<typename T> static constexpr bool ze::reflection::is_refl_type<std::vector<T>> = true;
template<typename T> static constexpr const char* ze::reflection::type_name<std::vector<T>> = "std::vector";

namespace ze::reflection
{

static void initialize_reflection_data() ZE_USED;

}