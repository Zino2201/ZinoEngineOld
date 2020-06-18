#include "BasePass.hlsl"

struct VSInput
{
	float4 Position : POSITION;
	float3 Normal : NORMAL;
};

VSOutput Main(VSInput Input)
{
	float4x4 WVP = mul(World, ViewProj);

	Input.Position.w = 1.0f;

	VSOutput Out;
	Out.Position = mul(Input.Position, WVP);
	Out.WorldNormal = mul(Input.Normal, World);

	// Fresnel
	float3 I = normalize(ViewPos);
	float FresnelScale = 1;
	float FresnelPower = 0.6;
	Out.Fresnel = FresnelScale * pow(1.0 + dot(I, Out.WorldNormal), FresnelPower);

	return Out;
}