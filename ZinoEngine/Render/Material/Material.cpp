#include "Material.h"
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include "IO/IOUtils.h"
#include "Core/Engine.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/Renderer/ForwardSceneRenderer.h"
#include "Render/Commands/Commands.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/RenderSystem/Vulkan/VulkanRenderSystem.h"
#include "Render/RenderSystem/Vulkan/VulkanDevice.h"

void CMaterialRenderData::Init(CMaterial* InMaterial)
{
	Material = InMaterial;
}

void CMaterialRenderData::InitRenderThread()
{
	std::vector<SShaderParameter> ShaderParameters;
	for (const auto& Value : Material->ShaderParameterMap)
	{
		ShaderParameters.insert(ShaderParameters.end(), Value.second.begin(), Value.second.end());
	}

	/** Create pipeline */
	Pipeline = g_Engine->GetRenderSystem()->CreateGraphicsPipeline(
		SRenderSystemGraphicsPipelineInfos(
		Material->ShaderMap[EShaderStage::Vertex].get(),
		Material->ShaderMap[EShaderStage::Fragment].get(),
		SVertex::GetBindingDescription(),
		SVertex::GetAttributeDescriptions(),
		ShaderParameters));

	UniformBuffer = g_Engine->GetRenderSystem()->CreateUniformBuffer(
		SRenderSystemUniformBufferInfos(48));
}

void CMaterialRenderData::DestroyRenderThread()
{
	UniformBuffer->Destroy();
}

CMaterial::~CMaterial() 
{ 
	RenderData->DestroyResources();
	RenderData->GetDestroyedSemaphore().Wait();
	delete RenderData;
}

void CMaterial::Load(const std::string& InPath)
{
	RenderData = new CMaterialRenderData;
	 
	std::string Json = IOUtils::ReadTextFile(InPath);

	/** Parse Json */
	rapidjson::Document Document;
	if(Document.Parse(Json.c_str()).HasParseError())
	{
		LOG(ELogSeverity::Error, "Failed to parse %s: %s", InPath.c_str(), 
			GetParseError_En(Document.GetParseError()));
		return;
	}
	
	/** Get shaders array */
	rapidjson::Value& Shaders = Document["shaders"];
	for(rapidjson::SizeType i = 0; i < Shaders.Size(); ++i)
	{
		std::string StageStr = Shaders[i]["stage"].GetString();
		std::string Shader = Shaders[i]["shader"].GetString();
		
		/** Try cast Stage to EShaderStage */
		auto StageVal = magic_enum::enum_cast<EShaderStage>(StageStr);
		if(StageVal.has_value())
		{
			EShaderStage Stage = StageVal.value();

			/** Create shader */
			ShaderMap.insert(std::make_pair(Stage,
				g_Engine->GetRenderSystem()->CreateShader(
				IOUtils::ReadBinaryFile("Assets/Shaders/" + Shader).data(),
				IOUtils::ReadBinaryFile("Assets/Shaders/" + Shader).size(),
				Stage)));

			/** Shader attributes */
			ShaderParameterMap.insert(std::make_pair(Stage,
				ParseShaderJson(Stage, "Assets/Shaders/" + Shader + ".json")));
		}
		else
		{
			LOG(ELogSeverity::Error, "Invalid shader stage");
			return;
		}
	}

	RenderData->Init(this);
	RenderData->InitResources();
}

std::vector<SShaderParameter> CMaterial::ParseShaderJson(const EShaderStage& InStage,
	const std::string& InPath)
{
	std::vector<SShaderParameter> ShaderAttributes;

	std::string Json = IOUtils::ReadTextFile(InPath);

	/** Parse Json */
	rapidjson::Document Document;
	if (Document.Parse(Json.c_str()).HasParseError())
	{
		LOG(ELogSeverity::Error, "Failed to parse %s: %s", InPath.c_str(),
			GetParseError_En(Document.GetParseError()));
		return {};
	}

	rapidjson::Value& Attributes = Document["attributes"];
	ShaderAttributes.reserve(Attributes.Size());

	for(rapidjson::SizeType i = 0; i < Attributes.Size(); ++i)
	{
		std::string Name = Attributes[i]["name"].GetString();
		uint32_t Binding = Attributes[i]["binding"].GetUint();

		uint64_t Size = Attributes[i]["size"].GetUint64();
		size_t UboAlignment = g_VulkanRenderSystem->GetDevice()->GetPhysicalDevice()
			.getProperties().limits.minUniformBufferOffsetAlignment;
		uint64_t AlignedSize = static_cast<uint32_t>((Size / UboAlignment) * UboAlignment 
			+ ((Size % UboAlignment) > 0 ? UboAlignment : 0));

		std::string TypeStr = Attributes[i]["type"].GetString();
		uint32_t Set = Attributes[i]["set"].GetUint();
		uint32_t Count = Attributes[i]["count"].GetUint();

		auto TypeVal = magic_enum::enum_cast<EShaderParameterType>(TypeStr);
		if (TypeVal.has_value())
		{
			EShaderParameterType Type = TypeVal.value();
			ShaderAttributes.push_back({Name, Type, Set, Binding, Size, Count, InStage });
		}
		else
		{
			LOG(ELogSeverity::Error, "Invalid attribute type");
			return {};
		}
	}

	return ShaderAttributes;
}

void CMaterial::SetMaterialUBO(const void* InNewData, const uint64_t& InSize)
{
	EnqueueRenderCommand([this, InNewData, InSize](CRenderCommandList* InCommandList)
	{
		void* Mem = RenderData->GetUniformBuffer()->Map();
		memcpy(Mem, InNewData, InSize);
		RenderData->GetUniformBuffer()->Unmap();
	});
}