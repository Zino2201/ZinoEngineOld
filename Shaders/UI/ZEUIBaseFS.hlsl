#include "UI/ZEUIBase.hlsl"

float4 fragment(VSOutput input) : SV_TARGET
{
    return float4(input.Color, 1);
}