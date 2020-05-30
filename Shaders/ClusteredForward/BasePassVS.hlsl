#include "BasePass.hlsl"

struct VSInput
{
	float4 Position : POSITION;
	float4 Color : COLOR;
};

VSOutput Main(VSInput Input)
{
	float4x4 WVP = mul(World, ViewProj);

	Input.Position.w = 1.0f;

	VSOutput Out;
	Out.Position = mul(Input.Position, WVP);
	Out.Color = Input.Color;
	return Out;
}