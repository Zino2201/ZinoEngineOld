// UI fragment shader

layout(location = 0) in vec2 FragUV;
layout(location = 1) in vec4 FragColor;

layout(location = 0) out vec4 OutColor;

layout(set = 0, binding = 1) uniform sampler2D Font;

void main()
{
    OutColor = FragColor * texture(Font, FragUV);
}