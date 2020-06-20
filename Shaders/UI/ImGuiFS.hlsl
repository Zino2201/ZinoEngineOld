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
Texture2D Font;

[[vk::binding(2)]]
SamplerState Sampler;

float4 Main(VSOutput Input) : SV_TARGET
{
    return Input.Color * Font.Sample(Sampler, Input.TexCoord);
}