#pragma once

#include "DrawCommand.h"

namespace ze::ui
{

struct DrawCommandPrimitiveRect final : public DrawCommandPrimitive
{
	void build(const DrawCommand& commmand) override;
};

}