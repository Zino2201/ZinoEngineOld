#include "BasePass.hlsl"

float4 Main(VSOutput Input) : SV_TARGET
{
	return float4(Input.Position.z, Input.Position.z, Input.Position.z, 1);
}