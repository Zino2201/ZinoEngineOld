#include "Material.h"
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include "IO/IOUtils.h"
#include "Core/Engine.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/Renderer/Renderer.h"
#include "Render/ShaderAttributesManager.h"
#include "Render/Commands/RenderCommands.h"
#include "Render/RenderSystem/Vulkan/VulkanRenderSystem.h"
#include "Render/RenderSystem/Vulkan/VulkanDevice.h"

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
				IOUtils::ReadBinaryFile("Assets/Shaders/" + Shader).data(),
				IOUtils::ReadBinaryFile("Assets/Shaders/" + Shader).size(),
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
	Pipeline = CEngine::Get().GetRenderSystem()->CreateGraphicsPipeline(SRenderSystemGraphicsPipelineInfos(
		ShaderMap[EShaderStage::Vertex].get(),
		ShaderMap[EShaderStage::Fragment].get(),
		SVertex::GetBindingDescription(),
		SVertex::GetAttributeDescriptions(),
		ShaderAttributes));

	AttributeManager = Pipeline->CreateShaderAttributesManager(EShaderAttributeFrequency::PerMaterial);

	/** Associate buffers with attributes */
	for(const auto& ShaderAttribute : ShaderAttributes)
	{
		if (ShaderAttribute.Type == EShaderAttributeType::UniformBufferStatic
			&& ShaderAttribute.Frequency == EShaderAttributeFrequency::PerMaterial)
			AttributeManager->Set(ShaderAttribute.StageFlags, 
				ShaderAttribute.Name, AttributeBufferMap[ShaderAttribute]->GetBuffer());
	}
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
		std::string FrequencyStr = Attributes[i]["frequency"].GetString();
		uint32_t Count = Attributes[i]["count"].GetUint();
		rapidjson::Value& Meta = Attributes[i]["meta"];
		std::vector<SShaderAttributeMember> AttributeMembers;

		auto TypeVal = magic_enum::enum_cast<EShaderAttributeType>(TypeStr);
		if (TypeVal.has_value())
		{
			if(Attributes[i].HasMember("members"))
			{
				rapidjson::Value& Members = Attributes[i]["members"];
				AttributeMembers.reserve(Members.Size());

				for (rapidjson::SizeType i = 0; i < Members.Size(); ++i)
				{
					std::string MemberName = Members[i]["name"].GetString();
					uint64_t MemberSize = Members[i]["size"].GetUint64();
					uint64_t MemberOffset = Members[i]["offset"].GetUint64();

					AttributeMembers.emplace_back(MemberName, MemberSize, MemberOffset);
				}
			}

			auto FrequencyVal = magic_enum::enum_cast<EShaderAttributeFrequency>(FrequencyStr);
			if(FrequencyVal.has_value())
			{
				EShaderAttributeType Type = TypeVal.value();
				EShaderAttributeFrequency Frequency = FrequencyVal.value();
				EShaderAttributeMeta Meta;

				ShaderAttributes.emplace_back(Name, Binding, AlignedSize, Type, Frequency, InStage, Count,
					Meta, AttributeMembers);
				if (Type == EShaderAttributeType::UniformBufferStatic
					&& Frequency == EShaderAttributeFrequency::PerMaterial)
				{
					IRenderSystemUniformBufferPtr UniformBuffer =
						CEngine::Get().GetRenderSystem()->CreateUniformBuffer(SRenderSystemUniformBufferInfos(Size));
					AttributeBufferMap.insert(std::make_pair(ShaderAttributes.back(), UniformBuffer));
				}
			}
			else
			{
				LOG(ELogSeverity::Error, "Invalid frequency");
				return {};
			}
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
	AttributeManager->Set(InStage, InName, InResource);
}

void CMaterial::SetUniformData(const std::string& InName,
	const void* InData, uint64_t InOffset, uint64_t InSize)
{
	for (const SShaderAttribute& Attribute : 
		Pipeline->GetShaderAttributes(EShaderAttributeFrequency::PerMaterial))
	{
		if (Attribute.Name == InName)
		{
			if(Attribute.Type != EShaderAttributeType::UniformBufferStatic)
				continue;

			auto FindResult = AttributeBufferMap.find(Attribute);
			if (FindResult != AttributeBufferMap.end())
			{
				IRenderSystemUniformBufferPtr& UniformBuffer = FindResult->second;

				if (UniformBuffer->GetInfos().bUsePersistentMapping)
				{
					void* Dst = UniformBuffer->GetMappedMemory();
					char* DstCh = reinterpret_cast<char*>(Dst); // Use char* for arithmetic
					memcpy(DstCh + InOffset, 
						InData, 
						InSize == 0 ? Attribute.Size : InSize);
				}
				else
				{
					void* Dst = UniformBuffer->Map();
					char* DstCh = reinterpret_cast<char*>(Dst); // Use char* for arithmetic
					memcpy(DstCh + InOffset, 
						InData,
						InSize == 0 ? Attribute.Size : InSize);
					UniformBuffer->Unmap();
				}

				return;
			}
		}
	}
}

void CMaterial::SetTexture(const EShaderStage& InStage, const std::string& InName,
	IDeviceResource* InDeviceResource)
{
	AttributeManager->Set(InStage, InName, InDeviceResource);
}

void CMaterial::SetUniformBuffer(const std::string& InName,
	void* InData)
{
	SetUniformData(InName, InData);
}

void CMaterial::SetFloat(const std::string& InMemberName, const float& InData)	
{
	for (const SShaderAttribute& Attribute : 
		Pipeline->GetShaderAttributes(EShaderAttributeFrequency::PerMaterial))
	{
		for(const SShaderAttributeMember& Member : Attribute.Members)
		{
			if(Member.Name == InMemberName)
			{
				SetUniformData(Attribute.Name, reinterpret_cast<const void*>(&InData),
					Member.Offset, Member.Size);
				return;
			}
		}
	}
}

void CMaterial::SetVec3(const std::string& InMemberName, const glm::vec3& InData)
{
	for (const SShaderAttribute& Attribute :
		Pipeline->GetShaderAttributes(EShaderAttributeFrequency::PerMaterial))
	{
		for (const SShaderAttributeMember& Member : Attribute.Members)
		{
			if (Member.Name == InMemberName)
			{
				SetUniformData(Attribute.Name, reinterpret_cast<const void*>(&InData),
					Member.Offset, Member.Size);
				return;
			}
		}
	}
}