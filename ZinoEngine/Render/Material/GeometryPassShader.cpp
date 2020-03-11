#include "GeometryPassShader.h"

DECLARE_SHADER_CLASS(CGeometryPassShaderVS, "GeometryPassShaderVS", 
	"Assets/Shaders/Material/Types/GeometryPass.vert",
	EShaderStage::Vertex)

DECLARE_SHADER_CLASS(CGeometryPassShaderFS, "GeometryPassShaderFS", 
	"Assets/Shaders/Material/Types/GeometryPass.frag",
	EShaderStage::Fragment)

CGeometryPassShaderVS::CGeometryPassShaderVS(CShaderClass* InClass,
	const SCompiledShaderData& InData) : CMaterialShader(InClass, InData) {}
CGeometryPassShaderFS::CGeometryPassShaderFS(CShaderClass* InClass,
	const SCompiledShaderData& InData) : CMaterialShader(InClass, InData) {}