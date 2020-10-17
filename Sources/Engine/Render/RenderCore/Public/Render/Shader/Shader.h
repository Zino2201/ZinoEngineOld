#pragma once

#include "EngineCore.h"
#include "NonCopyable.h"
#include "Shader/ShaderCompiler.h"
#include "Render/RenderSystem/RenderSystem.h"
#include <robin_hood.h>

namespace ze::gfx::shaders
{

class CShader;

/**
 * Base shader type class
 */
class RENDERCORE_API CShaderType
{
public:
    using InstantiateFunctionType = CShader*(*)(const CShaderType* InShaderType,
        const ShaderCompilerOutput& InOutput);

    CShaderType(const char* InName, const char* InFilename,	const char* InEntryPoint,
        ShaderStage InStage,
        InstantiateFunctionType InFunc);

    virtual CShader* InstantiateShader(const ShaderCompilerOutput& InOutput) const;

    const char* GetName() const { return Name; }
    const char* GetFilename() const { return Filename; }
    const char* GetEntryPoint() const { return EntryPoint; }
    ShaderStage GetStage() const { return Stage; }
    InstantiateFunctionType GetInstantiateFunc() const { return InstantiateFunc; }
private:
    const char* Name;
    const char* Filename;
    const char* EntryPoint;
    ShaderStage Stage;
    InstantiateFunctionType InstantiateFunc;
};

/**
 * Base shader instance class
 */
class RENDERCORE_API CShader
    : public boost::intrusive_ref_counter<CShader, boost::thread_unsafe_counter>
{
public:
    CShader(const CShaderType* InShaderType, const ShaderCompilerOutput& InOutput);
    virtual ~CShader();
    
    CRSShader* GetShader() const { return Shader.get(); }
private:
	CRSShaderPtr Shader;
};

using CShaderPtr = boost::intrusive_ptr<CShader>;

/**
 * Shader manager singleton
 */
class RENDERCORE_API CShaderManager : public CNonCopyable
{
public:
    static CShaderManager& Get()
    {
        static CShaderManager Instance;
        return Instance;
    }

    void AddShaderType(const char* InName, CShaderType* InShaderType);
private:
    robin_hood::unordered_map<const char*, CShaderType*> ShaderMap;
};

#define DECLARE_SHADER(Class, BaseClass) \
    public: \
    using ShaderType = BaseClass##Type; \
    static ShaderType ShaderStaticType; \
    inline static const char* ShaderNameType = #Class; \
    static gfx::shaders::CShader* InstantiateShader(const gfx::shaders::CShaderType* InType, const gfx::shaders::ShaderCompilerOutput& InOutput) \
    { \
        return new Class(InType, InOutput); \
    }

#define IMPLEMENT_SHADER(Shader, Name, Filename, EntryPoint, Stage) \
    Shader::ShaderType Shader::ShaderStaticType(Name, Filename, EntryPoint, Stage, \
        Shader::InstantiateShader);

}