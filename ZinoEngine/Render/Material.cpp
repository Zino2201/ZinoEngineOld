#include "Material.h"
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include "IO/IOUtils.h"
#include "Core/Engine.h"
#include "Render/RenderSystem.h"
#include "Render/Pipeline.h"
#include "Render/Shader.h"
#include "Render/ShaderAttributesManager.h"

void CMaterial::Load(const std::string& InPath)
{
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
				CEngine::Get().GetRenderSystem()->CreateShader(
				IOUtils::ReadBinaryFile("Assets/Shaders/" + Shader),
				Stage)));

			/** Shader attributes */
			ShaderAttributeMap.insert(std::make_pair(Stage,
				ParseShaderJson(Stage, "Assets/Shaders/" + Shader + ".json")));
		}
		else
		{
			LOG(ELogSeverity::Error, "Invalid shader stage");
			return;
		}
	}

	std::vector<SShaderAttribute> ShaderAttributes;
	for(const auto& Value : ShaderAttributeMap)
	{
		ShaderAttributes.insert(ShaderAttributes.end(), Value.second.begin(), Value.second.end());
	}

	/** Create pipeline */
	Pipeline = CEngine::Get().GetRenderSystem()->CreateGraphicsPipeline(SGraphicsPipelineInfos(
		ShaderMap[EShaderStage::Vertex].get(),
		ShaderMap[EShaderStage::Fragment].get(),
		SVertex::GetBindingDescription(),
		SVertex::GetAttributeDescriptions(),
		ShaderAttributes));
}

std::vector<SShaderAttribute> CMaterial::ParseShaderJson(const EShaderStage& InStage,
	const std::string& InPath)
{
	std::vector<SShaderAttribute> ShaderAttributes;

	std::string Json = IOUtils::ReadTextFile(InPath);

	/** Parse Json */
	rapidjson::Document Document;
	if (Document.Parse(Json.c_str()).HasParseError())
	{
		LOG(ELogSeverity::Error, "Faied to parse %s: %s", InPath.c_str(),
			GetParseError_En(Document.GetParseError()));
		return {};
	}

	rapidjson::Value& Attributes = Document["attributes"];
	ShaderAttributes.reserve(Attributes.Size());

	for(rapidjson::SizeType i = 0; i < Attributes.Size(); ++i)
	{
		std::string Name = Attributes[i]["name"].GetString();
		uint32_t Binding = Attributes[i]["binding"].GetUint();
		uint64_t Size = Attributes[i]["binding"].GetUint64();
		std::string TypeStr = Attributes[i]["type"].GetString();
		uint32_t Count = Attributes[i]["count"].GetUint();

		auto TypeVal = magic_enum::enum_cast<EShaderAttributeType>(TypeStr);
		if (TypeVal.has_value())
		{
			EShaderAttributeType Type = TypeVal.value();
			ShaderAttributes.emplace_back(Name, Binding, Type, InStage, Count);
		}
		else
		{
			LOG(ELogSeverity::Error, "Invalid attribute type");
			return {};
		}
	}

	return ShaderAttributes;
}

void CMaterial::SetShaderAttributeResource(const EShaderStage& InStage,
	const std::string& InName, IDeviceResource* InResource)
{
	Pipeline->GetShaderAttributesManager()->Set(InStage, InName, InResource);
}