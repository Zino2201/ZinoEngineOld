#pragma once

#include <type_traits>
#include <cstdint>
#include "Detail/AnyImpl.h"

namespace ze::reflection
{

/**
 * Custom std::any type that store a reflected type instance
 */
class Any
{
public:
	Any() : visit_func(&detail::AnyPolicyEmpty::visit) {}

	template<typename T, typename Decayed = std::decay_t<T>>
		requires IsReflType<Decayed>
	Any(T&& value) :
		visit_func(&detail::AnyPolicy<Decayed>::visit)
	{
		static_assert(std::is_copy_constructible_v<Decayed>, 
			"The type must be copy constructible. Please provide a copy constructor.");
		detail::AnyPolicy<Decayed>::create(std::forward<T>(value), data);
	}

	~Any() { visit_func(detail::VisitType::Destroy, data); }

	/**
	 * Get the stored value
	 * WARNING: Not type-safe
	 */
	template<typename T>
	ZE_FORCEINLINE const T& get_value()
	{
		std::any val = visit_func(detail::VisitType::GetValue, data);
		return **std::any_cast<const T*>(&val);
	}

	ZE_FORCEINLINE const Type* get_type()
	{
		std::any val = visit_func(detail::VisitType::GetType, data);
		return *std::any_cast<const Type*>(&val);
	}

	ZE_FORCEINLINE bool has_value() const 
	{ 
		std::any val = visit_func(detail::VisitType::IsValid, 
			const_cast<detail::AnyDataType&>(data));
		return *std::any_cast<bool>(&val);
	}
private:
	detail::AnyDataType data;
	detail::VisitFunc visit_func;
};

}