#include "BasePass.hlsl"

float4 Main(VSOutput Input) : SV_TARGET
{
	float4 FresnelColor = float4(0.4, 0.825, 1, 1);
    float4 FresnelColor2 = float4(0, 108 / 255, 153 / 255, 1);
	return lerp(FresnelColor2, FresnelColor, Input.Fresnel);
}