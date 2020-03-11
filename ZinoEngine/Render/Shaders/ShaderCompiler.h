#pragma once

#include "Render/RenderCore.h"

/**
 * Data representing a compiled shader
 */
struct SCompiledShaderData
{   
    /** Out SPIR-V */
    std::vector<unsigned int> OutSpv;

    /** Parameters */
    std::vector<SShaderParameter> Parameters;
};

/**
 * Shader compiler static class
 */
class CShaderCompiler
{
public:
    /**
     * Compile the specified shader
     * Blocking
     */
    static SCompiledShaderData CompileShader(EShaderStage InStage, 
        const std::string& InPath,
        const std::string& InEntryPoint);
};