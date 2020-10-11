#pragma once

#include "Type.h"
#include "Class.h"
#include "Enum.h"
#include "Registration.h"
#include <type_traits>
#include "Reflection/Detail/PropertyImpl.h"

namespace ze::reflection::builders
{

#define ZE_REFL_BUILDER_FUNC(UniqueName) ZE_REFL_BUILDER_FUNC_INTERNAL(UniqueName)
#define ZE_REFL_BUILDER_FUNC_INTERNAL(c) \
    struct ZE_CONCAT(_Refl_AutoInit, c) \
    { \
        ZE_CONCAT(_Refl_AutoInit, c) () \
	    { \
            ze::reflection::initialize_reflection_data(); \
        } \
    }; \
    static ZE_CONCAT(_Refl_AutoInit, c) ZE_CONCAT(Refl_AutoInitStruct, c); \
    static void initialize_reflection_data()

/**
 * Builder for registering and building a type
 */
template<typename T, typename U = Type>
struct TypeBuilder
{
	TypeBuilder(const char* in_name) 
	{
		TypeFlags flags;

		/** Flags */
		if constexpr(std::is_arithmetic_v<T>)
			flags |= TypeFlagBits::Arithmetic; 

		if constexpr(IsReflClass<T>)
			flags |= TypeFlagBits::Class; 

		if constexpr(IsReflEnum<T>)
			flags |= TypeFlagBits::Enum; 

		type = RegistrationManager::get().register_type(new U(in_name, sizeof(T), flags));
	}

	const Type* type;
};

/**
 * Builder for registering and building a struct/class
 */
template<typename T>
struct ClassBuilder : public TypeBuilder<T, Class>
{
	ClassBuilder(const char* in_name) : TypeBuilder<T, Class>(in_name) 
	{
		class_ = static_cast<const Class*>(type);

		auto& class_flags = const_cast<ClassFlags&>(class_->get_class_flags());
		if constexpr(std::is_abstract_v<T>)
			class_flags |= ClassFlagBits::Abstract;
	}

	ClassBuilder& parent(const std::string& in_name)
	{
		LazyTypePtr& parent = const_cast<LazyTypePtr&>(class_->get_parent_lazy_ptr());
		parent = LazyTypePtr(in_name);
		return *this;
	}

	template<typename PropType, typename PropPtr>
	ClassBuilder& property(const std::string& in_name, PropPtr&& in_ptr, 
		const PropertyFlags& in_flags = PropertyFlagBits::None)
	{
		std::vector<ze::reflection::Property>& properties = 
			const_cast<std::vector<ze::reflection::Property>&>(class_->get_propreties());
		properties.emplace_back(in_name, type_name<PropType>, 
			(char*)&((T*)nullptr->*in_ptr) - (char*)nullptr, in_flags);

		auto& property_impl = const_cast<std::unique_ptr<detail::PropertyImplBase>&>(properties.back().get_impl());
		property_impl = std::make_unique<detail::PropertyImplMember<PropType>>(properties.back().get_offset());

		return *this;
	}

	template<typename... Args>
	ClassBuilder& constructor()
	{
		if constexpr(!std::is_abstract_v<T>)
		{
			std::vector<ze::reflection::Constructor>& constructors = 
				const_cast<std::vector<ze::reflection::Constructor>&>(class_->get_constructors());
			constructors.push_back(ze::reflection::Constructor::make_constructor<T, Args...>());
		}
		
		return *this;
	}

	const Class* class_;
};

template<typename T>
struct EnumBuilder : public TypeBuilder<T, Enum>
{
public:
    using UnderlyingType = typename std::underlying_type<T>::type;

    EnumBuilder(const char* in_name) : 
		TypeBuilder<T, Enum>(in_name)
    {
        enum_ = static_cast<const Enum*>(type);

        auto& underlying_type = const_cast<LazyTypePtr&>(enum_->get_underlying_type_lazy_ptr());
		underlying_type = LazyTypePtr(type_name<UnderlyingType>);
    }

    template<typename E>
	EnumBuilder& value(const std::string& in_name,
        const E& in_enum)
    {
		auto& values = 
			const_cast<std::vector<std::pair<std::string, Any>>&>(enum_->get_values());

		values.push_back({ in_name, static_cast<UnderlyingType>(in_enum) });

        return *this;
    }

	const Enum* enum_;
};

}