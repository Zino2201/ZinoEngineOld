#include "UI/ZEUIBase.hlsl"

float4 fragment(VSOutput input) : SV_TARGET
{
    return input.Color;
}