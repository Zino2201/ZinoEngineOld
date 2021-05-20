#pragma once

#include <type_traits>
#include <cstdint>
#include "detail/AnyImpl.h"
#include <array>
#include "Serialization.h"

namespace ze::reflection
{

/**
 * Custom std::any type that store a reflected type instance
 */
class Any
{
public:
	Any() : visit_func(&detail::AnyPolicyEmpty::visit) {}

	Any(const Any& in_other)
		: visit_func(in_other.visit_func)
	{
		memcpy(&data, &in_other.data, sizeof(data));
	}

	template<typename T, typename Decayed = std::decay_t<T>>
		requires is_refl_type<Decayed>
	Any(T&& value) :
		visit_func(&detail::AnyPolicy<Decayed>::visit)
	{
		static_assert(std::is_copy_constructible_v<Decayed>, 
			"The type must be copy constructible. Please provide a copy constructor.");
		detail::AnyPolicy<Decayed>::create(std::forward<T>(value), data);
	}

	~Any() { visit_func(detail::VisitType::Destroy, data); }

	/**
	 * Serialize the value contained inside Any
	 * \warning The archive must be registered with ZE_REFL_SERL_REGISTER_ARCHIVE and the type registered with ZE_REFL_SERL_REGISTER_TYPE !
	 */
	template<typename ArchiveType>
	ZE_FORCEINLINE void serialize(ArchiveType& in_archive)
	{
		std::tuple<const char*, void*, detail::AnyDataType&> tuple = { serialization::ArchiveName<ArchiveType>, &in_archive, data };
		visit_func(detail::VisitType::Serialize, 
			*reinterpret_cast<detail::AnyDataType*>(&tuple));
	}
	
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

	
	ZE_FORCEINLINE void* get_value_ptr() const
	{
		std::any val = visit_func(detail::VisitType::GetValuePtr, data);
		return *std::any_cast<void*>(&val);
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

	ZE_FORCEINLINE std::string to_string() const
	{
		std::any val = visit_func(detail::VisitType::ToString, 
			const_cast<detail::AnyDataType&>(data));
		return *std::any_cast<std::string>(&val);
	}

	ZE_FORCEINLINE bool operator==(const Any& in_other) const
	{
		std::array<const Any*, 2> anys = { this, &in_other };
		std::any val = visit_func(detail::VisitType::Equals, *reinterpret_cast<detail::AnyDataType*>(&anys));
		return *std::any_cast<bool>(&val);
	}

	ZE_FORCEINLINE bool operator!=(const Any& in_other) const
	{
		std::array<const Any*, 2> anys = { this, &in_other };
		std::any val = visit_func(detail::VisitType::NotEquals, *reinterpret_cast<detail::AnyDataType*>(&anys));
		return *std::any_cast<bool>(&val);
	}
private:
	mutable detail::AnyDataType data;
	detail::VisitFunc visit_func;
};

}