#pragma once

#include "EngineCore.h"

namespace ze::reflection { class Type; }

namespace ze::editor
{

/** 
 * Interface for property editiors classes. These class defines how to edit a specific type.
 */
class PropertyEditor
{
public:
	/**
	 * Draw the property editor
	 * \param in_label Label to use
	 * \param in_value Property to edit
	 * \return True if the value has been changed
	 */
	virtual bool draw(const char* in_label, void* in_property) = 0;
};

/** 
 * Register the property editor for a specific type
 */
void register_property_editor(const reflection::Type* in_type, OwnerPtr<PropertyEditor> in_editor);
PropertyEditor* get_property_editor(const reflection::Type* in_type);

}