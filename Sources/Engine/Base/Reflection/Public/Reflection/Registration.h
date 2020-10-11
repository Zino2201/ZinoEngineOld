#pragma once

#include "EngineCore.h"
#include "Macros.h"
#include <robin_hood.h>

namespace ze::reflection
{

class Type;
class Class;

/**
 * Registration manager singleton
 * Store types
 * WARNING: Only one per module on modular builds, RegistrationManager::get() 
 *	will return the RegistrationManager for the current module
 */
class REFLECTION_API RegistrationManager
{
public:
	/**
	 * Register a type (will take ownership of the type)
	 */
	const Type* register_type(TOwnerPtr<Type> in_type);

	/**
	 * Tries to get the specified type
	 */
	const Type* get_type(const std::string& in_name) const;

	/**
	 * Get the registration manager for the specified module (on modular builds)
	 */
	static RegistrationManager& get()
	{
		static RegistrationManager Instance;
		return Instance;
	}

	RegistrationManager(const RegistrationManager&) = delete;
	void operator=(const RegistrationManager&) = delete;

	const auto& get_types() const { return types; }
	const auto& get_classes() const { return classes; }
private:
	RegistrationManager();
	~RegistrationManager();
private:
	std::vector<std::unique_ptr<Type>> types;
	std::vector<const Class*> classes;
	robin_hood::unordered_map<std::string, const Type*> type_name_to_ptr;
};

REFLECTION_API const std::vector<RegistrationManager*> get_registration_managers();
REFLECTION_API void register_registration_mgr(RegistrationManager* in_mgr);
REFLECTION_API void unregister_registration_mgr(RegistrationManager* in_mgr);

}