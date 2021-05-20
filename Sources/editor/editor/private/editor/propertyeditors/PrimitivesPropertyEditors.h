#pragma once

#include "editor/PropertyEditor.h"

namespace ze::editor
{

class BoolPropertyEditor final : public PropertyEditor
{
public:
	bool draw(const char* in_label, void* in_property) override;
};

}