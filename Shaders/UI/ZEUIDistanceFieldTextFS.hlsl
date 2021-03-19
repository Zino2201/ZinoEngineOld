#include "UI/ZEUIBase.hlsl"

[[vk::binding(0, 1)]]
SamplerState Sampler;

[[vk::binding(1, 1)]]
Texture2D Font;

[[vk::binding(2, 1)]]
cbuffer FontData
{
    /** Pixel range used to generate the SDF */
    float DistanceFieldRange;
};

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

float4 fragment(VSOutput input) : SV_TARGET
{
    float3 msdf = Font.Sample(Sampler, input.TexCoord).rgb;
    float dist = median(msdf.r, msdf.g, msdf.b);
    float screen_px_dist = DistanceFieldRange * (dist - 0.5);
    float alpha = clamp(screen_px_dist + 0.5, 0.0, 1.0);
    
    return float4(input.Color.r, input.Color.g, input.Color.b, alpha * input.Color.a);
}