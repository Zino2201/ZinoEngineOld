#pragma once

#include "Core/EngineCore.h"

/**
 * Shader stages
 */
enum class EShaderStage
{
	Vertex,
	Fragment
};

/** Vertex input rate */
enum class EVertexInputRate
{
	Vertex,
	Instance
};

/** Format */
enum class EFormat
{
	R32G32Sfloat,
	R32G32B32Sfloat,
};

/** Index format */
enum class EIndexFormat
{
	Uint16,
	Uint32
};

/**
 * Buffer usage
 */
enum class EBufferUsage
{
	VertexBuffer = 1 << 0,
	IndexBuffer	= 1 << 1,
	TransferSrc = 1 << 2,
	TransferDst = 1 << 3
};
DECLARE_FLAG_ENUM(EBufferUsage)

enum class EBufferMemoryUsage
{
	CpuOnly,
	GpuOnly,
	CpuToGpu,
	GpuToCpu
};

/**
 * Max frames in flight
 */
const uint32_t g_MaxFramesInFlight = 2;