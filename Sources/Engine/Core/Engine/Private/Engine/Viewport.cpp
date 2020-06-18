#include "Engine/Viewport.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/RenderSystem/RenderSystemContext.h"

namespace ZE
{

CViewport::CViewport(void* InWindowHandle, const uint32_t& InWidth,
	const uint32_t& InHeight) : WindowHandle(InWindowHandle), Width(InWidth),
		Height(InHeight) {}

void CViewport::InitResource_RenderThread()
{
	Surface = GRenderSystem->CreateSurface(
		WindowHandle, Width,
		Height, {});
	if(!Surface)
		LOG(ELogSeverity::Fatal, Viewport, "Failed to create viewport");
}

void CViewport::DestroyResource_RenderThread()
{
	Surface.reset(nullptr);
}

bool CViewport::Begin()
{
	must(IsInRenderThread());

	return GRSContext->BeginSurface(Surface.get());
}

void CViewport::End()
{
	must(IsInRenderThread());

	/** Present viewport surface */
	GRSContext->PresentSurface(Surface.get());
}

void CViewport::Resize(const uint32_t& InWidth, const uint32_t& InHeight)
{
	must(IsInRenderThread());
	
	Width = InWidth;
	Height = InHeight;

	Surface->Resize(InWidth, InHeight);
}

} /* namespace ZE */