#pragma once

#include "Reflection/Class.h"
#include <map>

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

	void draw();
private:
	const ze::reflection::Class* refl_class;
	void* object;
	std::map<std::string, Category> categories;
};

}