#pragma once

#include "EngineCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include <robin_hood.h>

namespace ZE::Renderer
{

class CRenderableComponentProxy;

/**
 * Transient renderer structures to store data about each renderable proxy
 */

/**
 * Per frame data about a proxy
 */
struct STransientProxyDataPerFrame
{
	const CRenderableComponentProxy* Proxy;

	STransientProxyDataPerFrame() : Proxy(nullptr) {}
};

using TransientPerFrameDataMap = robin_hood::unordered_map<const CRenderableComponentProxy*, 
	STransientProxyDataPerFrame>;

/**
 * Proxy data per view
 */
struct STransientProxyDataPerView
{
	STransientProxyDataPerFrame& PerFrameData;
	Math::SMatrix4 World;

	STransientProxyDataPerView(STransientProxyDataPerFrame& InPerFrameData) 
		: PerFrameData(InPerFrameData) {}
};

}