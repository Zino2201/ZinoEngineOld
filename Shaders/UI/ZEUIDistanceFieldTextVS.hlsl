#include "UI/ZEUIBase.hlsl"

struct VSInput
{
    float2 Position : POSITION;
    float3 Color : COLOR;
    float2 TexCoord : TEXCOORD;
};

[[vk::binding(0)]]
cbuffer GlobalData : register(b0, space0)
{
    float2 Scale;
    float2 Translate;
};

VSOutput vertex(VSInput Input)
{
    VSOutput Output;
    Output.Position = float4(Input.Position * Scale + Translate, 0.0f, 1.0f);
    Output.Color = Input.Color;
    Output.TexCoord = Input.TexCoord;
    return Output;
}