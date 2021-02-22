#include "UI/ZEUIBase.hlsl"

[[vk::binding(0)]]
Texture2D Font;

[[vk::binding(1)]]
SamplerState Sampler;

const float smoothing = 1.0 / 16.0;

float4 fragment(VSOutput input) : SV_TARGET
{
    float dist = Font.Sample(Sampler, input.TexCoord).r;
    float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, dist);
    
    return float4(input.Color, alpha);
}