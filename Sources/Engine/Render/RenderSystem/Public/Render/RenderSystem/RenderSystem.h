#pragma once

#include "EngineCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"

namespace ZE
{

class IRenderCommandContext;

RENDERSYSTEM_API extern class IRenderSystem* GRenderSystem;
RENDERSYSTEM_API extern class IRenderSystemContext* GRSContext;

/**
 * Render system interface
 */
class IRenderSystem
{
public:
	virtual ~IRenderSystem() = default;

	virtual void Initialize() = 0;
	virtual void Destroy() = 0;
    virtual void PrepareFrame() = 0;
    virtual void Present() = 0;
    virtual void WaitGPU() = 0;

	virtual CRSSurface* CreateSurface(void* InWindowHandle,
		const uint32_t& InWidth, const uint32_t& InHeight,
		const SRSResourceCreateInfo& InInfo = {}) const = 0;
	
	virtual CRSBuffer* CreateBuffer(
		const ERSBufferUsage& InUsageFlags,
		const ERSMemoryUsage& InMemUsage,
		const uint64_t& InSize,
		const SRSResourceCreateInfo& InInfo) const = 0;

	virtual CRSTexture* CreateTexture(
		const ERSTextureType& InTextureType,
		const ERSTextureUsage& InTextureUsage,
		const ERSMemoryUsage& InMemoryUsage,
		const EFormat& InFormat,
		const uint32_t& InWidth,
		const uint32_t& InHeight,
		const uint32_t& InDepth,
		const uint32_t& InArraySize,
		const uint32_t& InMipLevels,
		const ESampleCount& InSampleCount,
		const SRSResourceCreateInfo& InInfo = {}) const = 0;

	virtual CRSShader* CreateShader(
		const EShaderStage& InStage,
		const uint64_t& InBytecodeSize,
		const void* InBytecode,
		const SShaderParameterMap& InParameterMap,
		const SRSResourceCreateInfo& InCreateInfo = {}) const = 0;

	virtual CRSGraphicsPipeline* CreateGraphicsPipeline(
		const std::vector<SRSPipelineShaderStage>& InShaderStages,
		const std::vector<SVertexInputBindingDescription>& InBindingDescriptions,
		const std::vector<SVertexInputAttributeDescription>& InAttributeDescriptions,
		const SRSRenderPass& InRenderPass = SRSRenderPass(),
		const SRSBlendState& InBlendState = SRSBlendState(),
		const SRSRasterizerState& InRasterizerState = SRSRasterizerState(),
		const SRSDepthStencilState& InDepthStencilState = SRSDepthStencilState(),
		const SRSResourceCreateInfo& InCreateInfo = SRSResourceCreateInfo()) const = 0;

	/** Utils */
	virtual const char* GetName() const = 0;

 //   /** Factories */
 //   virtual IRenderCommandContext* CreateRenderCommandContext() const = 0;
 //   /**
	// * Create a shader
	// */
	//virtual CRenderSystemShaderPtr CreateShader(void* InData,
	//	size_t InDataSize,
	//	const EShaderStage& InShaderStage) = 0;

	///**
	// * Create a buffer
	// */
	//virtual CRenderSystemBufferPtr CreateBuffer(const SRenderSystemBufferInfos& InInfos) = 0;

	///**
	// * Create a vertex buffer
	// */
	//virtual IRenderSystemVertexBufferPtr CreateVertexBuffer(const uint64_t& InSize,
	//	EBufferMemoryUsage InMemoryUsage = EBufferMemoryUsage::GpuOnly,
	//	bool bUsePersistentMapping = false,
	//	const std::string& InDebugName = "VertexBuffer") = 0;
	//
	///**
	// * Create a index buffer
	// */
	//virtual IRenderSystemIndexBufferPtr CreateIndexBuffer(const uint64_t& InSize,
	//	EBufferMemoryUsage InMemoryUsage = EBufferMemoryUsage::GpuOnly,
	//	bool bUsePersistentMapping = false,
	//	const std::string& InDebugName = "IndexBuffer") = 0;

	///**
	// * Create a graphics pipeline
	// */
	//virtual IRenderSystemGraphicsPipelinePtr CreateGraphicsPipeline(const SRenderSystemGraphicsPipelineInfos& InInfos) = 0;

	///** 
	// * Create a uniform buffer 
	// */
	//virtual IRenderSystemUniformBufferPtr CreateUniformBuffer(const SRenderSystemUniformBufferInfos& InInfos) = 0;

	///**
	// * Create a texture
	// */
	//virtual CRenderSystemTexturePtr CreateTexture(const SRenderSystemTextureInfo& InInfos) = 0;

	///**
	// * Create a texture view
	// */
	//virtual CRenderSystemTextureViewPtr CreateTextureView(const SRenderSystemTextureViewInfo& InInfos) = 0;

	///*
	// * Create a sampler
	// */
	//virtual CRenderSystemSamplerPtr CreateSampler(const SRenderSystemSamplerInfo& InInfos) = 0;

	//virtual SRenderSystemDetails GetRenderSystemDetails() const = 0;

	//virtual TMulticastDelegate<>& GetFrameCompletedDelegate() = 0;
};

}