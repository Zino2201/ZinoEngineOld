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
Texture2D Font : register(t0, space0);

[[vk::binding(2)]]
SamplerState Sampler : register(t1, space0);

float4 Main(VSOutput Input) : SV_TARGET
{
    return Input.Color * Font.Sample(Sampler, Input.TexCoord);
}