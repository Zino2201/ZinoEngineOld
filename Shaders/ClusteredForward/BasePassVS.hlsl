#include "BasePass.hlsl"

struct VSInput
{
	float4 Position : POSITION;
	float4 Color : COLOR;
};

cbuffer PerInstance
{
    float4x4 WVP;
};

VSOutput Main(VSInput Input)
{
	VSOutput Out;
	Out.Position = mul(Input.Position, WVP);
	Out.Color = Input.Color;
	return Out;
}