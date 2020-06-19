#include "ZE.hlsl"

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

[[vk::binding(0, ZE_GLOBAL_SET)]]
Texture2D Color : register(t0);

[[vk::binding(1, ZE_GLOBAL_SET)]]
SamplerState Sampler : register(s1);

float4 Main(VSOutput Input) : SV_TARGET
{
    return Color.Sample(Sampler, Input.TexCoord);
}