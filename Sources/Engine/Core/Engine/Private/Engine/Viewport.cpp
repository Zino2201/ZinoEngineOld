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

	/** Create depth buffer */
	DepthBuffer = GRenderSystem->CreateTexture(
		ERSTextureType::Tex2D,
		ERSTextureUsage::DepthStencil,
		ERSMemoryUsage::DeviceLocal,
		EFormat::D32SfloatS8Uint,
		Width,
		Height,
		1,
		1,
		1,
		ESampleCount::Sample1);
}

void CViewport::DestroyResource_RenderThread()
{
	Surface.reset(nullptr);
}

void CViewport::Begin()
{
	must(IsInRenderThread());

	GRSContext->BeginSurface(Surface.get());
}

void CViewport::End()
{
	must(IsInRenderThread());

	/** Present viewport surface */
	GRSContext->PresentSurface(Surface.get());
}

} /* namespace ZE */