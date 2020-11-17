#pragma once

#include "EngineCore.h"

namespace ze::reflection 
{ 

class Type; 
class Any;

}

namespace ze::editor
{

/**
 * Cosmetic settings
 */
struct PropertyEditorDrawParams
{
	
};

/**
 * Base interface class for reflected type editors
 * This allows to have a custom edtior display for a reflected type
 */
class PROPERTYEDITOR_API PropertyEditor
{
public:
	virtual void draw(const char* in_label, void* in_value, PropertyEditorDrawParams in_params = PropertyEditorDrawParams()) = 0;
};

PROPERTYEDITOR_API void register_property_editor(const reflection::Type* in_type, OwnerPtr<PropertyEditor> in_editor);
PROPERTYEDITOR_API PropertyEditor* get_property_edtior(const reflection::Type* in_type);

}