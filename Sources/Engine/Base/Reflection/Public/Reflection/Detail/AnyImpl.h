#pragma once

#include "Reflection/Type.h"
#include <type_traits>
#include <concepts>
#include <any>

namespace ze::reflection::detail
{

using AnyDataType = std::aligned_storage_t<sizeof(uintptr_t), alignof(uintptr_t)>;

template<typename T>
static constexpr bool FitInAny = sizeof(T) <= sizeof(uintptr_t) &&
	alignof(T) <= alignof(uintptr_t);

enum class VisitType
{
	Destroy,
	GetType,
	GetValue,
	IsValid,
	Equals,
	NotEquals
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
		{
			const T* ptr = &Policy::get_value(any_data);
			return std::make_any<const T*>(ptr);
		}
		case VisitType::Equals:
		{
			std::pair<const Any&, const Any&>* pair = reinterpret_cast<std::pair<const Any&, const Any&>*>(&any_data);
			const reflection::Type* left_type = pair->first.get_type();
			const reflection::Type* right_type = pair->second.get_type();
			
			if(left_type == right_type)
			{
				return Policy::equals(pair->first.get_value<T>(), 
					pair->second.get_value<T>());
			}
			else
			{
				if constexpr(std::is_arithmetic_v<T>)
				{
					if(right_type->is_arithmetic())
					{
						return Policy::equals(*reinterpret_cast<uint64_t*>(pair->first.get_value_ptr()), 
							*reinterpret_cast<uint64_t*>(pair->second.get_value_ptr()));
					}
				}
			}
		}
		case VisitType::NotEquals:
		{
			std::pair<const Any&, const Any&>* pair = reinterpret_cast<std::pair<const Any&, const Any&>*>(&any_data);
			const reflection::Type* left_type = pair->first.get_type();
			const reflection::Type* right_type = pair->second.get_type();
			
			if(left_type == right_type)
			{
				return Policy::not_equals(pair->first.get_value<T>(), 
					pair->second.get_value<T>());
			}
			else
			{
				if constexpr(std::is_arithmetic_v<T>)
				{
					if(right_type->is_arithmetic())
					{
						return Policy::not_equals(*reinterpret_cast<uint64_t*>(pair->first.get_value_ptr()), 
							*reinterpret_cast<uint64_t*>(pair->second.get_value_ptr()));
					}
				}
			}
		}
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
		case VisitType::Equals:
			return true;
		case VisitType::NotEquals:
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

	ZE_FORCEINLINE static bool equals(const T& left, const T& right)
	{
		return left == right;
	}

	ZE_FORCEINLINE static bool not_equals(const T& left, const T& right)
	{
		return left != right;
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

	ZE_FORCEINLINE static bool equals(const T& left, const T& right)
	{
		return left == right;
	}

	ZE_FORCEINLINE static bool not_equals(const T& left, const T& right)
	{
		return left != right;
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

	ZE_FORCEINLINE static bool equals(const T& left, const T& right)
	{
		return left == right;
	}

	ZE_FORCEINLINE static bool not_equals(const T& left, const T& right)
	{
		return left != right;
	}
};

/**
 * Determine the correct policy to use
 */
template<typename T> 
using AnyPolicy = std::conditional_t<std::is_arithmetic_v<T>, AnyPolicyArithmetic<T>,
		std::conditional_t<FitInAny<T>, AnyPolicySmall<T>, AnyPolicyLarge<T>>>;

}