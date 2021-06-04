/**
 * Effect for ImGui rendering
 */

struct VSInput
{
    float2 position : POSITION;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR;
};

[[vk::binding(0)]]
cbuffer GlobalData : register(b0, space0)
{
	float2 scale;
    float2 translate;
};

[[vk::binding(1)]]
SamplerState texture_sampler : register(t1, space0);

[[vk::binding(2)]]
Texture2D texture : register(t2, space0);

VSOutput vertex(VSInput input)
{
    VSOutput output;
    output.position = float4(input.position * scale + translate, 0.0f, 1.0f);
    output.texcoord = input.texcoord;
    output.color = input.color;

    return output;
}

float4 fragment(VSOutput input) : SV_TARGET
{
    return input.color * texture.Sample(texture_sampler, input.texcoord);
}