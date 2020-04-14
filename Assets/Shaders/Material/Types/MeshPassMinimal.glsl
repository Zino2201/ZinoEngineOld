// Minimal include for mesh passes

#include "Minimal.glsl"

// Informations about the view
struct SMeshPassViewData
{
	mat4 ProjView;
	vec3 ViewDir;
};

// Basic per-instance UBO
struct SProxyShaderPerInstanceData
{
	mat4 World;
};

// Uniform buffers & SSBOs

// Globals

layout(set = GLOBAL_SET, binding = 0) uniform GlobalSceneData
{
    SMeshPassViewData ViewData;
};

// Per instances

layout(std430, set = INSTANCE_SET, binding = 0) readonly restrict buffer PerInstanceData
{
    SProxyShaderPerInstanceData ProxyDatas[];
};