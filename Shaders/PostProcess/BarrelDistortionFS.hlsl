struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

Texture2D Color : register(t0);
SamplerState Sampler : register(s1);

float4 Main(VSOutput Input) : SV_TARGET
{
    return Color.Sample(Sampler, Input.TexCoord);
}