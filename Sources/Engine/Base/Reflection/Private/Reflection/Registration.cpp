#include "Reflection/Registration.h"
#include "Reflection/Type.h"

namespace ze::reflection
{

RegistrationManager::RegistrationManager()
{
	register_registration_mgr(this);
}

RegistrationManager::~RegistrationManager()
{
	unregister_registration_mgr(this);
}

const Type* RegistrationManager::register_type(TOwnerPtr<Type> in_type)
{
	types.emplace_back(in_type);
	type_name_to_ptr.insert({ types.back()->get_name(), types.back().get() });

	if(types.back()->is_class())
		classes.emplace_back(static_cast<const Class*>(types.back().get()));

	return types.back().get();
}

const Type* RegistrationManager::get_type(const std::string& in_name) const
{
	auto type = type_name_to_ptr.find(in_name);
	if(type != type_name_to_ptr.end())
		return type->second;

	return nullptr;
}

std::vector<RegistrationManager*> reg_mgrs;

void register_registration_mgr(RegistrationManager* in_mgr)
{
	reg_mgrs.emplace_back(in_mgr);
}

void unregister_registration_mgr(RegistrationManager* in_mgr)
{
	size_t idx = -1;
	for(size_t i = 0; i < reg_mgrs.size(); ++i)
	{
		if(reg_mgrs[i] == in_mgr)
		{
			idx = -1;
			break;
		}
	}
	
	if(idx != -1)
		reg_mgrs.erase(reg_mgrs.begin() + idx);
}

const std::vector<RegistrationManager*> get_registration_managers()
{
	return reg_mgrs;
}

}