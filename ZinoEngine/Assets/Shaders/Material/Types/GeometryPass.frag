#extension GL_ARB_separate_shader_objects : enable

#include "MeshPassMinimal.glsl"

layout(location = 0) in vec3 FragColor;
layout(location = 1) in vec2 FragTexCoord;
layout(location = 2) in vec3 FragNormal;
layout(location = 3) in vec3 FragPos;
layout(location = 4) in vec3 FragCamPos;
layout(location = 5) in vec3 FragWorldNormal;

layout(location = 0) out vec4 OutColor;

layout(set = INSTANCE_SET, binding = 1) uniform LightData
{
    DirectionalLight Sun;
};

#include "Temp/Material.glsl"

const float Ambient = 0.15;

void main() 
{   
    // Lighting
    vec3 LightColor = vec3(1, 1, 1);
    vec3 Normal = normalize(FragNormal);

    vec3 LightDir = normalize(Sun.Position - FragPos);
    float Diffuse = max(dot(Normal, LightDir), 0.0);
    vec3 DiffuseColor = Diffuse * LightColor;

    // Ambient lighting
    vec3 AmbientColor = LightColor * Ambient;

    vec3 Result = (AmbientColor + DiffuseColor) * GetMaterialOutColor();
	OutColor = vec4(Result, 1.0);
}