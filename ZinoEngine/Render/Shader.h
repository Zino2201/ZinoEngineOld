#pragma once

#include "RenderCore.h"

/** Shader object */
class IShader
{
public:
	IShader(const std::vector<uint8_t>& InData,
		const EShaderStage& InShaderStage) {}
	virtual ~IShader() = default;
};