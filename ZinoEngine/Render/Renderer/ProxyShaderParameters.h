#pragma once

/**
 * Per instance data for a proxy
 */
struct SProxyShaderPerInstanceData
{
	alignas(16) glm::mat4 World;
};