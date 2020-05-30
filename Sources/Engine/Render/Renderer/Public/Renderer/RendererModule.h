#pragma once

#include "Module/Module.h"
#include "Render/RenderSystem/RenderSystemResources.h"

namespace ZE::Renderer
{

struct SQuadVertex
{
	Math::SVector2f Position;
	Math::SVector2f TexCoord;

	SQuadVertex(const Math::SVector2f& InPosition,
		const Math::SVector2f& InTexCoord) : Position(InPosition), TexCoord(InTexCoord) {}
};

class CRendererModule : public CModule
{
public:
	void Initialize() override;
	void Destroy() override;

	static inline CRSBufferPtr QuadVBuffer;
	static inline CRSBufferPtr QuadIBuffer;
};

}