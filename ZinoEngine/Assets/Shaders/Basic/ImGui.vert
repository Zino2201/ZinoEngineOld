// UI vertex shader

layout(location = 0) in vec2 InPosition;
layout(location = 1) in vec2 InUV;
layout(location = 2) in vec4 InColor;

layout(location = 0) out vec2 FragUV;
layout(location = 1) out vec4 FragColor;

layout(set = 0, binding = 0) uniform PerInstanceData
{
	vec2 Scale;
	vec2 Translate;
};

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	gl_Position = vec4(InPosition * Scale + Translate, 0, 1);
	FragUV = InUV;
	FragColor = InColor;
}