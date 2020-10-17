#pragma once

#include "EngineCore.h"
#include "Resources/Resource.h"

namespace ze
{

/** Forward */
class CRSSurface;
class CRSBuffer;
class CRSTexture;
class CRSSampler;

struct SRSGraphicsPipeline;
struct SRSRenderPass;
struct SRSFramebuffer;
struct SRSSamplerCreateInfo;
struct SRSBufferCreateInfo;
struct SRSTextureCreateInfo;
namespace gfx::shaders {
class CRSShader;
struct SRSShaderCreateInfo;
using CRSShaderPtr = boost::intrusive_ptr<CRSShader>;
}
struct SRSSurfaceCreateInfo;

/** Intrusive ptrs */
using CRSSurfacePtr = boost::intrusive_ptr<CRSSurface>;
using CRSBufferPtr = boost::intrusive_ptr<CRSBuffer>;
using CRSTexturePtr = boost::intrusive_ptr<CRSTexture>;
using CRSSamplerPtr = boost::intrusive_ptr<CRSSampler>;

/** Index format */
enum class EIndexFormat
{
	Uint16,
	Uint32
};

/**
 * A viewport
 */
struct SViewport
{
	maths::Rect2D Rect;
	float MinDepth;
	float MaxDepth;
};

}