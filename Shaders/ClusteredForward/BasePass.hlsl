#include "ZE.hlsl"

struct VSOutput
{
	float4 Position : SV_POSITION;
	float3 WorldNormal : NORMAL;
	float Fresnel : COLOR;
};

cbuffer ViewData : register(b0, space0)
{
	float4x4 ViewProj;
	float3 ViewPos;
	float3 ViewForward;
};

cbuffer PerInstance : register(b1, space0)
{
    float4x4 World;
};