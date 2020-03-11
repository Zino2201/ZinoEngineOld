#pragma once

#include "Render/RenderSystem/RenderSystem.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/Shaders/ShaderCompiler.h"

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
    typedef CShader*(*InstantiateFunctionType)(CShaderClass*,
         const SCompiledShaderData& InData);

    CShaderClass(const std::string& InName,
        const std::string& InFilename,
        EShaderStage InStage,
        InstantiateFunctionType InInstantiateFunction);

    /**
     * Instantiate a new shader from this class
     */
    CShader* InstantiateShader(const SCompiledShaderData& InData);
    void AddShader(CShader* InShader);
    void RemoveShader(CShader* InShader);
    static std::map<std::string, CShaderClass*>& GetShaderClassMap();
    static void DestroyShaderClasses();
    void ClearShaders();

    const std::string& GetName() const { return Name; }
    const std::string& GetFilename() const { return Filename; }
    EShaderStage GetStage() const { return Stage; }

protected:
    /** Shader map, each shader class should have one */
    std::set<CShader*> Shaders;

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
	CShader(CShaderClass* InClass,
        const SCompiledShaderData& InData);
	virtual ~CShader();

    CRenderSystemShader* GetShader() const { return Shader.get(); }
    const std::vector<SShaderParameter>& GetParameters() const { return Parameters; }
    CShaderClass* GetClass() const { return ShaderClass; }
protected:
	CRenderSystemShaderPtr Shader;
    CShaderClass* ShaderClass;
    std::vector<SShaderParameter> Parameters;
};

/**
 * Declare a shader
 */
#define DECLARE_SHADER(Shader, ShaderClass) \
    public: \
    using ShaderClassType = ShaderClass; \
    static ShaderClass ShaderStaticClassType; \
    static CShader* InstantiateShader(CShaderClass* InClass, \
        const SCompiledShaderData& InData) \
    { \
        return new Shader(InClass, InData); \
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
 
/** 
 * Instantiate shader function by name 
 * (Blocking)
 */
template<typename T>
T* InstantiateShaderSync(const std::string& InClassName)
{
    CShaderClass* Class = CShaderClass::GetShaderClassMap()[InClassName];
   
    /** Start compiling shader */
    SCompiledShaderData Data = CShaderCompiler::CompileShader(Class->GetStage(),
        Class->GetFilename(),
        "main");

    return static_cast<T*>(Class->InstantiateShader(Data));
}

/**
 * Wrap multiple shaders and provide utils functions without ref counting them
 */
class CShaderMapNoRefCnt
{   
public:
    void AddShader(CShader* InShader);

    const SShaderParameter& GetParameterByName(const std::string& InName) const;

    CShader* GetShader(EShaderStage InStage)
    {
        return Shaders[InStage];
    }

    std::vector<SShaderParameter> GetParameters() const;

    const std::map<EShaderStage, CShader*>& GetMap() const
    {
        return Shaders;
    }
private:
    std::map<EShaderStage, CShader*> Shaders;
};

/**
 * Wrap multiple shaders and provide utils functions
 */
class CShaderMap
{   
public:
    void AddShader(CShader* InShader);

    const SShaderParameter& GetParameterByName(const std::string& InName) const;

    CShader* GetShader(EShaderStage InStage)
    {
        return Shaders[InStage].get();
    }

    std::vector<SShaderParameter> GetParameters() const;

    const std::map<EShaderStage, CShaderPtr>& GetMap() const
    {
        return Shaders;
    }
private:
    std::map<EShaderStage, CShaderPtr> Shaders;
};

/** 
 * Utils class for managing uniform buffers
 */

/**
 * A uniform buffer parameter
 * Wraps a shader parameter and a uniform buffer
 */
class CShaderParameterUniformBuffer
{
public:
	/**
	 * Bind at specified parameter
	 */
	void Bind(const SShaderParameter& InParameter);

	/**
	 * Use the specified uniform buffer
	 */
	void Use(class IRenderCommandContext* InContext);

    /**
     * Set a member
     */
    void SetMember(const std::string& InName, const void* InData);

	/**
	 * Destroy uniform buffer
	 */
	void Destroy();

    const SShaderParameter& GetShaderParameter() const { return Parameter; }
    IRenderSystemUniformBuffer* GetUniformBuffer() const { return UniformBuffer.get(); }
private:
	SShaderParameter Parameter;
	IRenderSystemUniformBufferPtr UniformBuffer;
};