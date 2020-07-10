#pragma once

#include "WorldView.h"

namespace ZE::Renderer
{

class CRenderableComponentProxy;
struct STransientProxyDataPerFrame;
struct STransientProxyDataPerView;

/**
 * Base interface for render component types that defines 
 *	the renderer type of a RenderableComponent.
 * It acts as an interface between the game and the renderer to control how to interpret the
 * renderable component data and how to draw it.
 */
class RENDERER_API IRenderableComponentType
{
public:
	/**
	 * Copy per frame component state
	 */
	virtual void CopyPerFrameComponentState(const CRenderableComponentProxy& InComponentProxy,
		STransientProxyDataPerFrame& OutPerFrameData) = 0;

	/**
	 * Copy component state to renderer transient per view version
	 * Called before actual rendering, after visibility
	 */
	virtual void CopyComponentStatePerView(const CRenderableComponentProxy& InComponentProxy,
		STransientProxyDataPerView& OutPerViewData) = 0;

	
};

RENDERER_API IRenderableComponentType* RegisterRenderableComponentType(
	TOwnerPtr<IRenderableComponentType> InComponentType);

template<typename T>
T* RegisterRenderableComponentType()
{
	return static_cast<T*>(RegisterRenderableComponentType(new T));
}

}