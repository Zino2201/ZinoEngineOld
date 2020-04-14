/** 
* Base Pass vertex shader
* To include with a material shader
*/

#extension GL_ARB_separate_shader_objects : enable

#include "MeshPassMinimal.glsl"

// Vertex attributes
layout(location = 0) in vec3 InPosition;
layout(location = 1) in vec3 InColor;	
layout(location = 2) in vec2 InTexCoord;
layout(location = 3) in vec3 InNormal;
layout(location = 4) in uint InInstanceIdx;

layout(location = 0) out vec3 FragColor;
layout(location = 1) out vec2 FragTexCoord;
layout(location = 2) out vec3 FragNormal;
layout(location = 3) out vec3 FragPos;
layout(location = 4) out vec3 FragCamPos;
layout(location = 5) out vec3 FragWorldNormal;

#include "Temp/Material.glsl"

out gl_PerVertex 
{
    vec4 gl_Position;
};

void main()
{
    gl_Position = ViewData.ProjView * 
		(ProxyDatas[InInstanceIdx].World * vec4(InPosition, 1.0) + vec4(GetMaterialVertexOffset(), 1.0));
	FragColor = InColor;
	FragTexCoord = InTexCoord;
	FragNormal = GetMaterialNormal();
	FragPos = vec3(ProxyDatas[InInstanceIdx].World * vec4(InPosition, 1.0));
	FragCamPos = ViewData.ViewDir;
	FragWorldNormal = vec3(ProxyDatas[InInstanceIdx].World * vec4(InNormal, 1.0));
}