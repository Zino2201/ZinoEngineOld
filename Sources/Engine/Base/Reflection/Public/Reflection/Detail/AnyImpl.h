#pragma once

#include "Reflection/Type.h"
#include <type_traits>
#include <concepts>
#include <any>

namespace ze::reflection::detail
{

using AnyDataType = std::aligned_storage_t<sizeof(intmax_t), alignof(intmax_t)>;

template<typename T>
static constexpr bool FitInAny = sizeof(T) <= sizeof(intmax_t) &&
	alignof(T) <= alignof(intmax_t);

enum class VisitType
{
	Destroy,
	GetType,
	GetValue,
	IsValid
};

using VisitFunc = std::any(*)(const VisitType&, AnyDataType&);

/**
 * Base class for Any policy
 */
template<typename T, typename Policy>
struct AnyPolicyBase 
{
	static std::any visit(const VisitType& type, AnyDataType& any_data)
	{
		switch(type)
		{
		case VisitType::Destroy:
			Policy::destroy(reinterpret_cast<T&>(any_data));
			break;
		case VisitType::GetType:
			return std::make_any<const Type*>(get_type());
		case VisitType::IsValid:
			return true;
		case VisitType::GetValue:
			const T* ptr = &Policy::get_value(any_data);
			return std::make_any<const T*>(ptr);
		}

		return nullptr;
	}

	ZE_FORCEINLINE static const Type* get_type()
	{
		return Type::get<T>();
	}
};

/**
 * Empty policy
 */
struct AnyPolicyEmpty
{
	static std::any visit(const VisitType& type, AnyDataType& any_data)
	{
		switch(type)
		{
		case VisitType::Destroy:
			break;
		case VisitType::GetType:
			return nullptr;
		case VisitType::GetValue:
			return nullptr;
		case VisitType::IsValid:
			return false;
		}

		return nullptr;
	}
};

/**
 * Policy for small types that can fit in Any
 */
template<typename T>
struct AnyPolicySmall : public AnyPolicyBase<T, AnyPolicySmall<T>>
{
	ZE_FORCEINLINE static const T& get_value(const AnyDataType& data_ptr)
    {
        return reinterpret_cast<const T&>(data_ptr);
    }

	template<typename U>
	ZE_FORCEINLINE static void create(U&& value, AnyDataType& data_ptr) 
	{
		new (&data_ptr) T(std::forward<U>(value));
	}

	ZE_FORCEINLINE static void destroy(T& value) 
	{
		value.~T();
	}
};

/**
 * Policy for large types that can't be fit into Any
 */
template<typename T>
struct AnyPolicyLarge : public AnyPolicyBase<T, AnyPolicyLarge<T>>
{
	ZE_FORCEINLINE static const T& get_value(const AnyDataType& data_ptr)
    {
        return *reinterpret_cast<T* const &>(data_ptr);
    }

	template<typename U>
	ZE_FORCEINLINE static void create(U&& value, AnyDataType& data_ptr) 
	{
		reinterpret_cast<T*&>(data_ptr) = new T(std::forward<U>(value));
	}

	ZE_FORCEINLINE static void destroy(T& value)
	{
		delete &value;
	}
};

/**
 * Policy for arithmetics types
 * Arithmetic types ALWAYS fit in an Any
 */
template<typename T>
struct AnyPolicyArithmetic : public AnyPolicyBase<T, AnyPolicyArithmetic<T>>
{
	ZE_FORCEINLINE static const T& get_value(const AnyDataType& data_ptr)
    {
        return reinterpret_cast<const T&>(data_ptr);
    }

	template<typename U>
	ZE_FORCEINLINE static void create(U&& value, AnyDataType& data_ptr) 
	{
		reinterpret_cast<T&>(data_ptr) = value;
	}

	ZE_FORCEINLINE static void destroy(T& value) {}
};

/**
 * Determine the correct policy to use
 */
template<typename T> 
using AnyPolicy = std::conditional_t<std::is_arithmetic_v<T>, AnyPolicyArithmetic<T>,
		std::conditional_t<FitInAny<T>, AnyPolicySmall<T>, AnyPolicyLarge<T>>>;

}