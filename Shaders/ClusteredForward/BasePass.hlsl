struct VSOutput
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
};

cbuffer ViewData
{
	float4x4 ViewProj;
};

cbuffer PerInstance
{
    float4x4 World;
};