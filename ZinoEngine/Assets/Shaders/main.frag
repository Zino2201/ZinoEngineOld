#version 450
#extension GL_ARB_separate_shader_objects : enable

#define PER_MATERIAL 0
#define PER_INSTANCE 1

layout(location = 0) in vec3 FragColor;
layout(location = 1) in vec2 FragTexCoord;
layout(location = 2) in vec3 FragNormal;
layout(location = 3) in vec3 FragPos;
layout(location = 4) in vec3 FragCamPos;

layout(location = 0) out vec4 OutColor;

layout(set = PER_MATERIAL, binding = 0) uniform sampler2D TexSampler;

layout(set = PER_MATERIAL, binding = 1) uniform MaterialData {
    vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	float Shininess;
    //vec2 _Pad01;
} Material;

layout(set = PER_INSTANCE, binding = 1) uniform Light {
    vec3 LightPos;
} LightUBO;

void main() 
{   
    // Diffuse color
    vec3 TexColor = vec3(texture(TexSampler, FragTexCoord));
    vec3 Normal = normalize(FragNormal);
    
    vec3 LightColor = vec3(1, 1, 1);

    // Diffuse lighting
    vec3 LightDir = normalize(LightUBO.LightPos  - FragPos);
    float Diffuse = max(dot(Normal, LightDir), 0.0);
    vec3 DiffuseColor = (Diffuse * Material.Diffuse) * LightColor;

    // Specular lighting
    vec3 ViewDir = normalize(FragCamPos - FragPos);
    vec3 ReflectDir = reflect(-LightDir, Normal);
    float Specular = pow(max(dot(ViewDir, ReflectDir), 0.0), Material.Shininess);
    vec3 SpecularColor = (Specular * Material.Specular) * LightColor;

    // Ambient lighting
    vec3 AmbientColor = LightColor * Material.Ambient;

    // Final color
    vec3 Result = (AmbientColor + DiffuseColor + SpecularColor) * TexColor;
	OutColor = vec4(Result, 1.0);
}