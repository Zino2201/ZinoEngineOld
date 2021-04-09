#pragma once

#include "Reflection/Class.h"
#include <map>
#include <robin_hood.h>
#include "Delegates/Delegate.h"

namespace ze::editor
{

/**
 * Allows to edit the properties of a reflected object easily 
 */
class PropertiesEditor
{
	struct Category
	{
		std::vector<const ze::reflection::Property*> properties;
	};

public:
	PropertiesEditor(const ze::reflection::Class* in_class, void* in_object);

	bool draw();
	void bind_to_value_changed(const std::string& in_property, const std::function<void(void*)>& in_func);
private:
	const ze::reflection::Class* refl_class;
	void* object;
	std::map<std::string, Category> categories;
	robin_hood::unordered_map<std::string, DelegateNoRet<void*>> on_value_changed_map;
};

}