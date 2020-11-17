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
	ZE_FORCEINLINE const T& get_value() const
	{
		std::any val = visit_func(detail::VisitType::GetValue, data);
		return **std::any_cast<const T*>(&val);
	}

	void* get_value_ptr() const
	{
		return &data;
	}

	ZE_FORCEINLINE const Type* get_type() const
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

	ZE_FORCEINLINE bool operator==(const Any& in_other) const
	{
		std::pair<const Any&, const Any&> pair = std::make_pair<const Any&, const Any&>(*this, in_other);
		std::any val = visit_func(detail::VisitType::Equals, *reinterpret_cast<detail::AnyDataType*>(&pair));
		return *std::any_cast<bool>(&val);
	}

	ZE_FORCEINLINE bool operator!=(const Any& in_other) const
	{
		std::pair<const Any&, const Any&> pair = std::make_pair<const Any&, const Any&>(*this, in_other);
		std::any val = visit_func(detail::VisitType::NotEquals, *reinterpret_cast<detail::AnyDataType*>(&pair));
		return *std::any_cast<bool>(&val);
	}
private:
	mutable detail::AnyDataType data;
	detail::VisitFunc visit_func;
};

}