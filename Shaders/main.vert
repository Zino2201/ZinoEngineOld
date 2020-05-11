#version 450

layout(location = 0) in vec3 InPos;
layout(location = 1) in vec3 InColor;

layout(location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform PerInstance
{
    mat4 WVP;
	mat4 World;
};

void main() {
    gl_Position = WVP * vec4(InPos, 1.0);
    fragColor = InColor;
}
