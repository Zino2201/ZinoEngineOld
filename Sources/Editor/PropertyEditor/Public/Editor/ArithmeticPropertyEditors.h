#pragma once

#include "Editor/PropertyEditor.h"

namespace ze::editor
{

/**
 * Type editors for arithmetic types
 */

class PROPERTYEDITOR_API BoolPropertyEditor final : public PropertyEditor
{
public:
	bool draw(const char* in_label, void* in_value, PropertyEditorDrawParams in_params) override;
};

class PROPERTYEDITOR_API Uint32PropertyEditor final : public PropertyEditor
{
public:
	bool draw(const char* in_label, void* in_value, PropertyEditorDrawParams in_params) override;
};

class PROPERTYEDITOR_API FloatPropertyEditor final : public PropertyEditor
{
public:
	bool draw(const char* in_label, void* in_value, PropertyEditorDrawParams in_params) override;
};

}