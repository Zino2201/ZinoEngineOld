#include "reflection/Class.h"
#include "reflection/Registration.h"

namespace ze::reflection
{

const Class* Class::get_by_name(const std::string& in_name)
{
	const Type* type = Type::get_by_name(in_name);
	if(type && type->is_class())
		return static_cast<const Class*>(type);

	return nullptr;
}

std::vector<const Class*> Class::get_derived_classes_from(const Class* in_class)
{
	std::vector<const Class*> classes;
	classes.reserve(10);

	for(const auto& reg_mgr : get_registration_managers())	
	{
		for (const auto& class_ : reg_mgr->get_classes())
		{
			if (class_ == in_class)
				continue;

			if (class_->is_derived_from(in_class))
				classes.push_back(class_);
		}
	}

	return classes;
}

bool Class::is_derived_from(const Class* in_other) const
{
	if(this == in_other)
		return true;

	if(parent.get_as_class() == in_other)
	{
		return true;
	}

	return false;
}

bool Class::is_base_of(const Class* in_other) const
{
	return this == in_other || in_other->get_parent() == this;
}


const Property* Class::get_property(const std::string& in_name) const
{
	for(const auto& property : properties)
	{
		if(property.get_name() == in_name)
			return &property;
	}

	return nullptr;
}

}