#include "Engine/Viewport.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/RenderSystem/RenderSystemContext.h"

namespace ZE
{

CViewport::CViewport(void* InWindowHandle, const uint32_t& InWidth,
	const uint32_t& InHeight) : WindowHandle(InWindowHandle), Width(InWidth),
		Height(InHeight) 
{
	Surface = GRenderSystem->CreateSurface(
		WindowHandle, Width,
		Height, {});
	if (!Surface)
		LOG(ELogSeverity::Fatal, Viewport, "Failed to create viewport");
}

bool CViewport::Begin()
{
	return GRSContext->BeginSurface(Surface.get());
}

void CViewport::End()
{
	/** Present viewport surface */
	GRSContext->PresentSurface(Surface.get());
}

void CViewport::Resize(const uint32_t& InWidth, const uint32_t& InHeight)
{
	Width = InWidth;
	Height = InHeight;

	Surface->Resize(InWidth, InHeight);
}

} /* namespace ZE */