/**
 * Barrel Distortion post-process shader
 * https://www.decarpentier.nl/lens-distortion
 */

struct VSInput
{
    float2 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

VSOutput Main(VSInput Input)
{
    VSOutput Output;
    Output.Position = float4(Input.Position, 0.0f, 1.0f);
    Output.TexCoord = Input.TexCoord;

    return Output;
}