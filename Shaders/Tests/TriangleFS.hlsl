struct VSOutput
{
    float4 Position : SV_Position;
    float3 Color : COLOR0;
};

float4 Main(VSOutput Input) : SV_TARGET
{
    return float4(Input.Color, 1.0);
}