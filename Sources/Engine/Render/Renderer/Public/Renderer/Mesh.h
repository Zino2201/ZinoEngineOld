#pragma once

#include "Render/RenderSystem/RenderSystemResources.h"

namespace ze::renderer
{

/**
 * Represents a single mesh that is stored by a proxy
 */
struct SMesh
{
	CRSBuffer* VertexBuffer;
	CRSBuffer* IndexBuffer;
	EIndexFormat IndexFormat;
	union
	{
		uint32_t VertexCount;
		uint32_t IndexCount;
	};
};

}