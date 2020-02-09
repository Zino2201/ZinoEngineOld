#version 450
#extension GL_ARB_separate_shader_objects : enable

#define PER_MATERIAL 0
#define PER_INSTANCE 1

layout(location = 0) in vec3 InPosition;
layout(location = 1) in vec3 InColor;	
layout(location = 2) in vec2 InTexCoord;
layout(location = 3) in vec3 InNormal;

layout(location = 0) out vec3 FragColor;
layout(location = 1) out vec2 FragTexCoord;
layout(location = 2) out vec3 FragNormal;
layout(location = 3) out vec3 FragPos;
layout(location = 4) out vec3 FragCamPos;

layout(set = PER_INSTANCE, binding = 0) uniform TestUBO
{
	mat4 World;
	mat4 View;
	mat4 Projection;
	vec3 CamPos;
} UBO;

out gl_PerVertex 
{
    vec4 gl_Position;
};

void main()
{
    gl_Position = UBO.Projection * UBO.View * UBO.World * vec4(InPosition, 1.0);
	FragColor = InColor;
	FragTexCoord = InTexCoord;
	FragNormal = InNormal;
	FragPos = vec3(UBO.World * vec4(InPosition, 1.0));
	FragCamPos = UBO.CamPos;
}