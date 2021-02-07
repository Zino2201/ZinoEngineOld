#pragma once

#include "DrawCommand.h"

namespace ze::ui
{

struct DrawCommandPrimitiveRect final : public DrawCommandPrimitive
{
	std::pair<std::vector<Vertex>, std::vector<uint32_t>> get_geometry(const DrawCommand& commmand) override;
};

}