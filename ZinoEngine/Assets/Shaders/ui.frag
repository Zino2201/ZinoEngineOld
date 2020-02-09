#version 450

layout(binding = 0) uniform sampler2D FontSampler;

layout(location = 0) in vec2 FragTexCoord;
layout(location = 0) out vec4 OutColor;

void main()
{
	OutColor = texture(FontSampler, FragTexCoord);
}