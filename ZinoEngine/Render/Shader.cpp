#include "Shader.h"
#include "Core/Engine.h"
#include "RenderSystem/RenderSystem.h"
#include <magic_enum.hpp>
#include "IO/IOUtils.h"
#include "Render/Commands/RenderCommandContext.h"

/**
 * Shader class map
 * Pointer
 */
static std::map<std::string, CShaderClass*>* g_ShaderClasses;

std::map<std::string, CShaderClass*>& CShaderClass::GetShaderClassMap()
{
	return *g_ShaderClasses;
}

void CShaderClass::DestroyShaderClasses()
{
	delete g_ShaderClasses;
}

CShaderClass::CShaderClass(const std::string& InName, const std::string& InFilename,
	EShaderStage InStage,
	InstantiateFunctionType InInstantiateFunction) 
	: Name(InName), Filename(InFilename), Stage(InStage), InstantiateFunction(InInstantiateFunction)
{
	if(!g_ShaderClasses)
		g_ShaderClasses = new std::map<std::string, CShaderClass*>;

	g_ShaderClasses->insert(std::make_pair(Name, this));
}

void CShaderClass::AddShader(CShader* InShader)
{
	Shaders.insert(InShader);
}

void CShaderClass::RemoveShader(CShader* InShader)
{
	Shaders.erase(InShader);
}

void CShaderClass::ClearShaders()
{
	Shaders.clear();
}

CShader* CShaderClass::InstantiateShader(const SCompiledShaderData& InData)
{
	CShader* Shader = InstantiateFunction(this, InData);
	AddShader(Shader);
	return Shader;
}

CShader::CShader(CShaderClass* InClass,
	const SCompiledShaderData& InData) : ShaderClass(InClass),
	Parameters(InData.Parameters)
{
	Shader = g_Engine->GetRenderSystem()->CreateShader(
		reinterpret_cast<void*>(const_cast<unsigned int*>(InData.OutSpv.data())),
		InData.OutSpv.size() * 4, ShaderClass->GetStage());
}

CShader::~CShader() { ShaderClass->RemoveShader(this); }

/** Shader map */

void CShaderMapNoRefCnt::AddShader(CShader* InShader)
{
	Shaders[InShader->GetClass()->GetStage()] = InShader;
}

std::vector<SShaderParameter> CShaderMapNoRefCnt::GetParameters() const
{
	std::vector<SShaderParameter> ShaderParameters;

	/*std::vector<SShaderParameter> Parameters;

	for (const auto& [Stage, Shader] : Shaders)
	{
		std::copy(Shader->GetParameters().begin(), Shader->GetParameters().end(),
			std::back_inserter(Parameters));
	}*/

	{
		/** Added parameters map (name, index) */
		std::map<std::string, size_t> AddedParameters;

		for (const auto& [Stage, Shader] : Shaders)
		{
			for (const auto& Parameter : Shader->GetParameters())
			{
				if (AddedParameters.count(Parameter.Name))
				{
					ShaderParameters[AddedParameters[Parameter.Name]].StageFlags |=
						Stage;
				}
				else
				{
					ShaderParameters.push_back(Parameter);
					AddedParameters[Parameter.Name] = ShaderParameters.size() - 1;
				}
			}
		}
	}

	return ShaderParameters;
}

const SShaderParameter& CShaderMapNoRefCnt::GetParameterByName(const std::string& InName) const
{
	for(const auto& [Stage, Shader] : Shaders)
	{
		for(const auto& Parameter : Shader->GetParameters())
		{
			if(Parameter.Name == InName)
				return Parameter;
		}
	}

	return SShaderParameter::GetNull();
}

void CShaderMap::AddShader(CShader* InShader)
{
	Shaders[InShader->GetClass()->GetStage()] = InShader;
}

std::vector<SShaderParameter> CShaderMap::GetParameters() const
{
	std::vector<SShaderParameter> ShaderParameters;

	/*std::vector<SShaderParameter> Parameters;

	for (const auto& [Stage, Shader] : Shaders)
	{
		std::copy(Shader->GetParameters().begin(), Shader->GetParameters().end(),
			std::back_inserter(Parameters));
	}*/

	{
		/** Added parameters map (name, index) */
		std::map<std::string, size_t> AddedParameters;

		for (const auto& [Stage, Shader] : Shaders)
		{
			for (const auto& Parameter : Shader->GetParameters())
			{
				if (AddedParameters.count(Parameter.Name))
				{
					ShaderParameters[AddedParameters[Parameter.Name]].StageFlags |=
						Stage;
				}
				else
				{
					ShaderParameters.push_back(Parameter);
					AddedParameters[Parameter.Name] = ShaderParameters.size() - 1;
				}
			}
		}
	}

	return ShaderParameters;
}

const SShaderParameter& CShaderMap::GetParameterByName(const std::string& InName) const
{
	for(const auto& [Stage, Shader] : Shaders)
	{
		for(const auto& Parameter : Shader->GetParameters())
		{
			if(Parameter.Name == InName)
				return Parameter;
		}
	}

	return SShaderParameter::GetNull();
}

/** Shader parameters classes */
void CShaderParameterUniformBuffer::Bind(const SShaderParameter& InParameter)
{
	Parameter = InParameter;

	UniformBuffer = g_Engine->GetRenderSystem()->CreateUniformBuffer(
		SRenderSystemUniformBufferInfos(InParameter.Size));
}

void CShaderParameterUniformBuffer::Use(IRenderCommandContext* InContext)
{
	InContext->SetShaderUniformBuffer(
		Parameter.Set, Parameter.Binding, UniformBuffer.get());
}

void CShaderParameterUniformBuffer::SetMember(const std::string& InName, const void* InData)
{
	SShaderParameterMember Member = Parameter.GetMember(InName);
	if(Member == SShaderParameterMember::GetNull())
		return;

	void* DstPtr = UniformBuffer->GetMappedMemory();

	if (!UniformBuffer->GetInfos().bUsePersistentMapping)
		DstPtr = UniformBuffer->Map();

	/** Use char* for arithmetics */
	char* Dst = reinterpret_cast<char*>(DstPtr);
	memcpy(Dst + Member.Offset, InData, Member.Size);

	if (!UniformBuffer->GetInfos().bUsePersistentMapping)
		UniformBuffer->Unmap();
}

void CShaderParameterUniformBuffer::Destroy()
{
	UniformBuffer->Destroy();
}