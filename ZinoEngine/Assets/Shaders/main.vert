#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 InPosition;
layout(location = 1) in vec3 InColor;	

layout(location = 0) out vec3 OutFragColor;

layout(binding = 0) uniform TestUBO
{
	mat4 World;
	mat4 View;
	mat4 Projection;
} UBO;

out gl_PerVertex 
{
    vec4 gl_Position;
};

void main()
{
    gl_Position = UBO.Projection * UBO.View * UBO.World * vec4(InPosition, 0.0, 1.0);
	OutFragColor = InColor;
}