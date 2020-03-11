#pragma once

#include "MaterialShader.h"

/**
 * Geometry pass shaders
 * Used for materials
 */
class CGeometryPassShaderVS : public CMaterialShader
{
	DECLARE_SHADER(CGeometryPassShaderVS, CMaterialShaderClass)

	CGeometryPassShaderVS(CShaderClass* InClass,
		const SCompiledShaderData& InData);
};

class CGeometryPassShaderFS : public CMaterialShader
{
	DECLARE_SHADER(CGeometryPassShaderFS, CMaterialShaderClass)
   
	CGeometryPassShaderFS(CShaderClass* InClass,
		const SCompiledShaderData& InData);
};

using CGeometryPassShaderVSPtr = boost::intrusive_ptr<CGeometryPassShaderVS>;
using CGeometryPassShaderFSPtr = boost::intrusive_ptr<CGeometryPassShaderVS>;