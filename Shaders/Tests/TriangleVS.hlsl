struct VSOutput
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR0;
};

[[vk::push_constant]]
struct PushConstant
{
    matrix WVP;
} PC;

VSOutput Main(uint VertIdx : SV_VertexID)
{
    float2 positions[3] =
    {
        float2(0.0, -0.5),
        float2(0.5, 0.5),
        float2(-0.5, 0.5)
    };
    
    float3 colors[3] =
    {
        float3(1.0, 0.0, 0.0),
        float3(0.0, 1.0, 0.0),
        float3(0.0, 0.0, 1.0)
    };
    
    VSOutput output;
    output.Position = mul(float4(positions[VertIdx], 0.0, 1.0), PC.WVP);
    output.Color = colors[VertIdx];
    return output;

}