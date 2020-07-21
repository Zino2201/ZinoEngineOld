#pragma once

#include "EngineCore.h"
#include "Resources/Resource.h"

namespace ZE
{

/** Forward */
class CRSSurface;
class CRSBuffer;
class CRSTexture;
class CRSShader;
class CRSSampler;

struct SRSGraphicsPipeline;
struct SRSRenderPass;
struct SRSFramebuffer;
struct SRSSamplerCreateInfo;
struct SRSBufferCreateInfo;
struct SRSTextureCreateInfo;
struct SRSShaderCreateInfo;
struct SRSSurfaceCreateInfo;

/** Intrusive ptrs */
using CRSSurfacePtr = boost::intrusive_ptr<CRSSurface>;
using CRSBufferPtr = boost::intrusive_ptr<CRSBuffer>;
using CRSTexturePtr = boost::intrusive_ptr<CRSTexture>;
using CRSShaderPtr = boost::intrusive_ptr<CRSShader>;
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
	Math::SRect2D Rect;
	float MinDepth;
	float MaxDepth;
};

}