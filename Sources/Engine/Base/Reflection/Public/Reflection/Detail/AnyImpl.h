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
	GetValuePtr,
	IsValid,
	Equals,
	NotEquals,
	Serialize,
	ToString
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
		{
			Policy::destroy(*reinterpret_cast<T* const &>(any_data));
			break;
		}
		case VisitType::GetType:
			return std::make_any<const Type*>(get_type());
		case VisitType::IsValid:
			return true;
		case VisitType::GetValue:
		{
			const T* ptr = &Policy::get_value(any_data);
			return std::make_any<const T*>(ptr);
		}
		case VisitType::GetValuePtr:
		{
			const T* ptr = &Policy::get_value(any_data);
			return std::make_any<void*>(reinterpret_cast<void*>(const_cast<T*>(ptr)));
		}
		case VisitType::Equals:
		{
			std::array<const Any*, 2>* anys = reinterpret_cast<std::array<const Any*, 2>*>(&any_data);
			const reflection::Type* left_type = anys->at(0)->get_type();
			const reflection::Type* right_type = anys->at(1)->get_type();
			
			if(left_type == right_type)
			{
				return std::make_any<bool>(Policy::equals(anys->at(0)->get_value<T>(), 
					anys->at(1)->get_value<T>()));
			}
			else
			{
				if constexpr(std::is_arithmetic_v<T>)
				{
					if(right_type->is_arithmetic())
					{
						return std::make_any<bool>(Policy::equals(*reinterpret_cast<uint64_t*>(anys->at(0)->get_value_ptr()), 
							*reinterpret_cast<uint64_t*>(anys->at(1)->get_value_ptr())));
					}
				}
			}
		}
		case VisitType::NotEquals:
		{
			std::array<const Any*, 2>* anys = reinterpret_cast<std::array<const Any*, 2>*>(&any_data);
			const reflection::Type* left_type = anys->at(0)->get_type();
			const reflection::Type* right_type = anys->at(1)->get_type();
			
			if(left_type == right_type)
			{
				return std::make_any<bool>(Policy::equals(anys->at(0)->get_value<T>(), 
					anys->at(1)->get_value<T>()));
			}
			else
			{
				if constexpr(std::is_arithmetic_v<T>)
				{
					if(right_type->is_arithmetic())
					{
						return std::make_any<bool>(Policy::not_equals(*reinterpret_cast<uint64_t*>(anys->at(0)->get_value_ptr()), 
							*reinterpret_cast<uint64_t*>(anys->at(1)->get_value_ptr())));
					}
				}
			}
		}
		case VisitType::Serialize:
		{
			std::tuple<const char*, void*, detail::AnyDataType&>* tuple = 
				reinterpret_cast<std::tuple<const char*, void*, detail::AnyDataType&>*>(&any_data);
			const T* ptr = &Policy::get_value(std::get<2>(*tuple));
			auto& map = serialization::get_archive_map(std::get<0>(*tuple));
			auto serializer = map.find(reflection::Type::get<T>()->get_name());
			ZE_CHECK(serializer != map.end());
			serializer->second(std::get<1>(*tuple), const_cast<T*>(ptr));
			break;
		}
		case VisitType::ToString:
		{
			ZE_CHECKF(false, "unimplemented");
			break;
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
		volatile T test = value;
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