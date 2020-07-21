#include "Engine/Viewport.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/RenderSystem/RenderSystemContext.h"
#include "Render/RenderSystem/Resources/Surface.h"

namespace ZE
{

CViewport::CViewport(void* InWindowHandle, const uint32_t& InWidth,
	const uint32_t& InHeight, const bool& bInVSync) : WindowHandle(InWindowHandle), Width(InWidth),
		Height(InHeight) 
{
	Surface = GRenderSystem->CreateSurface({
		WindowHandle, Width,
		Height, bInVSync});
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

void CViewport::SetVSync(const bool& bInVSync)
{
	Surface = GRenderSystem->CreateSurface({
		WindowHandle, Width,
		Height, bInVSync });
	if (!Surface)
		LOG(ELogSeverity::Fatal, Viewport, "Failed to create viewport");
}

} /* namespace ZE */