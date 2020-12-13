#pragma once

#include "Editor/PropertyEditor.h"

namespace ze::editor
{

class PROPERTYEDITOR_API Vector3dPropertyEditor final : public PropertyEditor
{
public:
	bool draw(const char* in_label, void* in_value, PropertyEditorDrawParams in_params) override;
};

class PROPERTYEDITOR_API Vector3fPropertyEditor final : public PropertyEditor
{
public:
	bool draw(const char* in_label, void* in_value, PropertyEditorDrawParams in_params) override;
};


}