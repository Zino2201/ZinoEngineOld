#include "ZE.hlsl"

struct VSOutput
{
	float4 Position : SV_POSITION;
	float3 WorldNormal : NORMAL;
	float Fresnel : COLOR;
};

[[vk::binding(0, ZE_GLOBAL_SET)]]
cbuffer ViewData : register(b0)
{
	float4x4 ViewProj;
	float3 ViewPos;
	float3 ViewForward;
};

[[vk::binding(1, ZE_INSTANCE_SET)]]
cbuffer PerInstance : register(b1)
{
    float4x4 World;
};