#pragma once

#include "Module/Module.h"

namespace ZE::Renderer
{

class CRendererModule : public CModule
{
public:
	void Destroy() override;
};

}