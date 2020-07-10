#pragma once

#include "Render/RenderSystem/RenderSystemResources.h"

namespace ZE::Renderer
{

/**
 * Represents a single mesh that is stored by a proxy
 */
struct SMesh
{
	CRSBufferPtr VertexBuffer;
	CRSBufferPtr IndexBuffer;
	EIndexFormat IndexFormat;
	union
	{
		uint32_t VertexCount;
		uint32_t IndexCount;
	};
};

}