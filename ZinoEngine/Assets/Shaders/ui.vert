#version 450

layout(location = 0) in vec2 InPos;
layout(location = 1) in vec2 InTexCoord;

layout(location = 0) out vec2 FragTexCoord;

layout(push_constant) uniform PushData {
	vec2 Position;
} PushConstants;

out gl_PerVertex 
{
	vec4 gl_Position;
};

void main()
{
	gl_Position = vec4(InPos + PushConstants.Position, 0.0, 1.0);
	FragTexCoord = InTexCoord;
}