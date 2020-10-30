/**
 * Vertex shader for ImGui rendering
 */

struct VSInput
{
    float2 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR;
};

[[vk::binding(0)]]
cbuffer GlobalData : register(b0, space0)
{
	float2 Scale;
    float2 Translate;
};

VSOutput Main(VSInput Input)
{
    VSOutput Output;
    Output.Position = float4(Input.Position * Scale + Translate, 0.0f, 1.0f);
    Output.TexCoord = Input.TexCoord;
    Output.Color = Input.Color;

    return Output;
}