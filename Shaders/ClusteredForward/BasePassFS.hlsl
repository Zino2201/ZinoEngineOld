#include "BasePass.hlsl"

float4 Main(VSOutput Input) : SV_TARGET
{
	return Input.Color;
}