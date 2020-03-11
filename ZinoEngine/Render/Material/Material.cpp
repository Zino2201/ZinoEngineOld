#include "Material.h"
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include "IO/IOUtils.h"
#include "Core/Engine.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/Renderer/ForwardSceneRenderer.h"
#include "Render/Commands/RenderCommandContext.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/RenderSystem/Vulkan/VulkanRenderSystem.h"
#include "Render/RenderSystem/Vulkan/VulkanDevice.h"
#include "Render/Material/GeometryPassShader.h"
#include <filesystem>
namespace fs = std::filesystem;

void CMaterialRenderData::Init(CMaterial* InMaterial,
	const std::vector<SMaterialShader>& InShaders)
{
	Material = InMaterial;
	Shaders = InShaders;
}

void CMaterialRenderData::InitRenderThread()
{
	/**
	 * When compiling a shader material
	 * We need to temporary copy the material shader at Shaders/Temp 
	 */
	for(const auto& InShader : Shaders)
	{
		if(!InShader.Shader.empty())
			fs::copy("Assets/Shaders/" + InShader.Shader, "Assets/Shaders/Temp/Material.glsl",
				fs::copy_options::overwrite_existing);

		LOG(ELogSeverity::Info, "Material shader: " + InShader.Shader);
		ShaderMap.AddShader(InstantiateShaderSync<CShader>(InShader.Type));
	}

	/** Create pipeline */
	Pipeline = g_Engine->GetRenderSystem()->CreateGraphicsPipeline(
		SRenderSystemGraphicsPipelineInfos(
		ShaderMap.GetShader(EShaderStage::Vertex)->GetShader(),
		ShaderMap.GetShader(EShaderStage::Fragment)->GetShader(),
		SVertex::GetBindingDescriptions(),
		SVertex::GetAttributeDescriptions(),
		ShaderMap.GetParameters(),
		SRenderSystemRasterizerState(
			false,
			false,
			EPolygonMode::Fill,
			ECullMode::Back,
			EFrontFace::CounterClockwise),
		SRenderSystemBlendState::GetDefault(),
		SRenderSystemDepthStencilState(
			true,
			true,
			EComparisonOp::Less)));

	Scalars.Bind(ShaderMap.GetParameterByName("ScalarsUBO"));
	Vec3s.Bind(ShaderMap.GetParameterByName("Vec3sUBO"));
}

void CMaterialRenderData::Bind(IRenderCommandContext* InContext)
{
	InContext->BindGraphicsPipeline(Pipeline.get());
	Scalars.SetMember("MonScalarDeOuf", &g_Engine->ElapsedTime);
	Scalars.Use(InContext);
	Vec3s.Use(InContext);
}

void CMaterialRenderData::DestroyRenderThread()
{
	Scalars.Destroy();
	Vec3s.Destroy();
}

CMaterial::~CMaterial() 
{ 
	RenderData->DestroyResources();
	RenderData->GetDestroyedSemaphore().Wait();
	delete RenderData;
}

void CMaterial::Load(const std::string& InPath)
{
	std::vector<SMaterialShader> Shaders;
	Shaders.reserve(2);

	/** Parse Json */
	std::string Json = IOUtils::ReadTextFile(InPath);

	/** Parse Json */
	rapidjson::Document Document;
	if (Document.Parse(Json.c_str()).HasParseError())
	{
		LOG(ELogSeverity::Error, "Failed to parse %s: %s", InPath.c_str(),
			GetParseError_En(Document.GetParseError()));
		return;
	}

	/** Get shaders array */
	rapidjson::Value& ShadersArray = Document["shaders"];
	for (rapidjson::SizeType i = 0; i < ShadersArray.Size(); ++i)
	{
		std::string StageStr = ShadersArray[i]["stage"].GetString();
		auto StageVal = magic_enum::enum_cast<EShaderStage>(StageStr);

		std::string Type = ShadersArray[i]["type"].GetString();
		std::string Shader = ShadersArray[i]["shader"].GetString();
		EShaderStage Stage = StageVal.value();

		Shaders.push_back({ Stage, Type, Shader });
	}

	RenderData = new CMaterialRenderData;

	RenderData->Init(this, Shaders);
	RenderData->InitResources();

	rapidjson::Value& DefaultsArray = Document["defaults"];

	std::map<std::string, float> Scalars;
	std::map<std::string, glm::vec3> Vec3s;

	for (rapidjson::Value::ConstMemberIterator It = DefaultsArray.MemberBegin(); 
		It != DefaultsArray.MemberEnd(); ++It) 
	{
		std::string Name = It->name.GetString();
		const rapidjson::Value& Default = It->value;

		if (Default.IsArray()) /** Assume it is a vec3 for now */
		{
			Vec3s[Name] = glm::vec3(
				Default[0].GetFloat(),
				Default[1].GetFloat(),
				Default[2].GetFloat());
		}
		else
		{
			Scalars[Name] = Default.GetFloat();
		}
	}

	EnqueueRenderCommand([this, Scalars, Vec3s](CRenderCommandList* InCommandList)
	{
		/** Read defaults and apply them */
		for(const auto& [Name, Data] : Scalars)
			RenderData->GetScalars().SetMember(Name, &Data);

		for (const auto& [Name, Data] : Vec3s)
			RenderData->GetVec3s().SetMember(Name, &Data);
	});
}