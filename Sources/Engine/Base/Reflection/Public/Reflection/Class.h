#pragma once

#include "Type.h"
#include "Property.h"
#include <any>

namespace ze::reflection
{

class REFLECTION_API Constructor
{
public:
	template<typename... Args>
	using InstantiateFunc = std::function<void*(Args...)>;
	
	template<typename... Args>
	using PlacementNewFunc = std::function<void(void*, Args...)>;

	template<typename T, typename... Args>
	static Constructor make_constructor()
	{
		Constructor ctor;
		std::any& placement_new = const_cast<std::any&>(ctor.get_placement_new_func());
		placement_new = std::make_any<PlacementNewFunc<Args...>>(
			[](void* in_ptr, Args&&... args)
			{
				new (in_ptr) T(std::forward<Args>(args)...);
			});

		std::any& instantiate_func = const_cast<std::any&>(ctor.get_instantiate_func());
		instantiate_func = std::make_any<InstantiateFunc<Args...>>(
			[](Args&&... args) -> void*
			{
				return new T(std::forward<Args>(args)...);
			});

		return ctor;
	}

	/**
	 * Try to call the stored constructor on the specified pointer
	 * Returns true if the constructeur has been called
	 */
	template<typename... Args>
	bool placement_new(void* in_ptr, Args&&... args) const
	{
		if(const PlacementNewFunc<Args...>* func = 
			std::any_cast<PlacementNewFunc<Args...>>(&placement_new_func))
		{
			(*func) (in_ptr, std::forward<Args>(args)...);
			return true;
		}

		return false;
	}

	/**
	 * Try to instantiate the stored type
	 */
	template<typename... Args>
	void* instantiate(Args&&... args) const
	{
		if(const InstantiateFunc<Args...>* func = 
			std::any_cast<InstantiateFunc<Args...>>(&instantiate_func))
		{
			return (*func) (std::forward<Args>(args)...);
		}

		return nullptr;
	}

	const std::any& get_instantiate_func() const { return instantiate_func; }
	const std::any& get_placement_new_func() const { return placement_new_func; }
private:
	std::any instantiate_func;
	std::any placement_new_func;
};

enum class ClassFlagBits
{
	None = 0,

	Abstract = 1 << 0
};
ENABLE_FLAG_ENUMS(ClassFlagBits, ClassFlags)

/**
 * Represents a struct or a class
 */
class REFLECTION_API Class : public Type
{
public:
	Class(const char* in_name,
		const size_t& in_size,
		const TypeFlags& in_flags) : Type(in_name, in_size, in_flags) {}

	template<typename T, typename... Args>
	T* instantiate(Args&&... args) const
	{
		for(const auto& ctor : constructors)
		{
			if(void* data = ctor.instantiate<Args...>(std::forward<Args>(args)...))
				return reinterpret_cast<T*>(data);
		}

		return nullptr;
	}

	template<typename... Args>
	bool placement_new(void* in_ptr, Args&&... args) const
	{
		for(const auto& ctor : constructors)
		{
			if(ctor.placement_new<Args...>(in_ptr, std::forward<Args>(args)...))
				return true;
		}

		return false;
	}

	bool is_derived_from(const Class* in_other) const;
	bool is_base_of(const Class* in_other) const;

	/**
	 * Get a class by name, returns nullptr if not found
	 */
	static const Class* get_by_name(const std::string& in_name);
	
	template<typename T>
	ZE_FORCEINLINE static const Class* get()
	{
		return get_by_name(type_name<T>);
	}

	static std::vector<const Class*> get_derived_classes_from(const Class* in_class);
	
	ZE_FORCEINLINE bool is_abstract() const { return static_cast<bool>(class_flags & ClassFlagBits::Abstract); }

	ZE_FORCEINLINE const std::vector<Property>& get_propreties() const { return properties; }
	ZE_FORCEINLINE const std::vector<Constructor>& get_constructors() const { return constructors; }
	ZE_FORCEINLINE const Class* get_parent() const { return parent.get_as_class(); }
	ZE_FORCEINLINE const LazyTypePtr& get_parent_lazy_ptr() const { return parent; }
	ZE_FORCEINLINE const ClassFlags& get_class_flags() const { return class_flags; }
private:
	std::vector<Constructor> constructors;
	std::vector<Property> properties;
	LazyTypePtr parent;
	ClassFlags class_flags;
};
}