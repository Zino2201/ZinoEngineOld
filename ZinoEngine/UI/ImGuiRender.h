#pragma once

#include "Core/EngineCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/Texture2D.h"
#include "Render/Shaders/BasicShader.h"

/**
 * ImGui render resource
 */
class CImGuiRender : public CRenderResource
{
public:
    virtual void InitRenderThread() override;
    virtual void DestroyRenderThread() override;

	void UpdateBuffers();
	void Draw(class IRenderCommandContext* InCommandContext);
private:
    CTexture Font;
	IRenderSystemGraphicsPipelinePtr Pipeline;
	IRenderSystemUniformBufferPtr UniformBuffer;
	IRenderSystemVertexBufferPtr VertexBuffer;
	IRenderSystemIndexBufferPtr IndexBuffer;
	uint64_t VertexCount;
	uint64_t IndexCount;
	uint64_t LastVertexSize;
	uint64_t LastIndexSize;
	CShaderMapNoRefCnt ShaderMap;
};

class CImGuiShaderVS : public CBasicShader
{
    DECLARE_SHADER(CImGuiShaderVS, CBasicShaderClass)

	CImGuiShaderVS(CShaderClass* InClass,
		const SCompiledShaderData& InData);
};

class CImGuiShaderFS : public CBasicShader
{
	DECLARE_SHADER(CImGuiShaderFS, CBasicShaderClass)

	CImGuiShaderFS(CShaderClass* InClass,
		const SCompiledShaderData& InData);
};