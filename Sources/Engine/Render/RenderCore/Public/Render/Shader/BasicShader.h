#pragma once

#include "Shader.h"
#include "NonCopyable.h"

namespace ZE
{

/**
 * Basic shaders are shaders that only can be instantiated once
 */

/**
 * Base shader type class for basics shaders
 */
class RENDERCORE_API CBasicShaderType : public CShaderType
{
public:
    CBasicShaderType(const char* InName, const char* InFilename, 
        const char* InEntryPoint,
        EShaderStage InStage,
        InstantiateFunctionType InFunc);
    ~CBasicShaderType();
public:
    inline static bool bBasicShadersCompiled = false;
};

/**
 * Basic shader class for basics shaders
 */
class RENDERCORE_API CBasicShader : public CShader
{
public:
    CBasicShader(const CShaderType* InShaderType,
        const SShaderCompilerOutput& InOutput) : CShader(InShaderType, InOutput) {}
};

class RENDERCORE_API CBasicShaderManager : public CNonCopyable
{
public:
    static CBasicShaderManager& Get()
    {
        static CBasicShaderManager Instance;
        return Instance;
    }

    void AddShaderType(CBasicShaderType* InShaderType);
    void RemoveShaderType(CBasicShaderType* InShaderType);
    void DestroyAll();

    /**
     * Compile basic shaders (blocking)
     */
    void CompileShaders();

    CShader* GetShader(const char* InType) { return ShaderMap[InType].get(); }
private:
    std::vector<CBasicShaderType*> ShaderTypes;
    std::unordered_map<const char*, CShaderPtr> ShaderMap;
};

}