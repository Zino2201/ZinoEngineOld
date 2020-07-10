#pragma once

#include <imgui.h>
#include "Module/Module.h"

namespace ZE::UI
{

class IMGUI_API CImGuiModule : public CModule
{
public:
	void Initialize() override;
	void Destroy() override;
};

}