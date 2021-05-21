/**
 * Fragment shader for ImGui rendering
 */

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR;
};

[[vk::binding(1)]]
SamplerState Sampler : register(t1, space0);

[[vk::binding(2)]]
Texture2D Texture : register(t0, space0);

float4 fragment(VSOutput Input) : SV_TARGET
{
    return Input.Color * Texture.Sample(Sampler, Input.TexCoord);
}