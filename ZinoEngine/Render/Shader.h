#pragma once

#include "Render/RenderSystem/RenderSystem.h"
#include "Render/RenderSystem/RenderSystemResources.h"

/** Forward declarations */
class CShader;
class CShaderClass;

using CShaderPtr = boost::intrusive_ptr<CShader>;

/**
 * A shader class
 * Store CShader
 */
class CShaderClass
{
public:
    typedef CShader*(*InstantiateFunctionType)(CShaderClass*);

    CShaderClass(const std::string& InName,
        const std::string& InFilename,
        EShaderStage InStage,
        InstantiateFunctionType InInstantiateFunction);

    /**
     * Instantiate a new shader from this class
     */
    CShader* InstantiateShader();
    void AddShader(CShader* InShader);
    static std::map<std::string, CShaderClass*>& GetShaderClassMap() { return ShaderClasses; }
    void ClearShaders();

    const std::string& GetName() const { return Name; }
    const std::string& GetFilename() const { return Filename; }
    EShaderStage GetStage() const { return Stage; }
protected:
	static std::map<std::string, CShaderClass*> ShaderClasses;

    /** Shader map, each shader class should have one */
    std::vector<CShaderPtr> Shaders;

    /** Shader name */
    std::string Name;

    /** Shader filename */
    std::string Filename;

    /** Shader stage */
    EShaderStage Stage;

    /** Function to call to instantiate a shader with this class */
    InstantiateFunctionType InstantiateFunction;
};

/** 
 * An shader class instance
 * Perform reflection and compile to the appropriate format for the RenderSystem
 * Assumes that InData is in GLSL format !
 */
class CShader : 
    public boost::intrusive_ref_counter<CShader, boost::thread_unsafe_counter>
{
public:
	CShader(CShaderClass* InClass);
	virtual ~CShader();

    CRenderSystemShader* GetShader() const { return Shader.get(); }
protected:
	CRenderSystemShaderPtr Shader;
    CShaderClass* ShaderClass;
};

/**
 * Declare a shader
 */
#define DECLARE_SHADER(Shader, ShaderClass) \
    public: \
    using ShaderClassType = ShaderClass; \
    static ShaderClass ShaderStaticClassType; \
    static CShader* InstantiateShader(CShaderClass* InClass) \
    { \
        return new Shader(InClass); \
    }

/** 
 * Declare a shader class (should be in .cpp !!)
 */
#define DECLARE_SHADER_CLASS(ShaderClass, Name, Filename, Stage) \
    ShaderClass::ShaderClassType ShaderClass::ShaderStaticClassType( \
        Name, \
        Filename, \
        Stage, \
        ShaderClass::InstantiateShader);