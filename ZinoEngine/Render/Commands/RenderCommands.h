#pragma once

#include "Commands.h"
#include "Render/RenderSystem/RenderSystemResources.h"

class IRenderSystemGraphicsPipeline;
class CRenderSystemBuffer;
class IShaderAttributesManager;
class IRenderSystemUniformBuffer;
class IDeviceResource;

/**
 * Begin command recording command
 */
class CRenderCommandBeginRecording : public IRenderCommand
{
public:
	virtual void Execute(CRenderCommandList* InCmdList) override;

	virtual std::string GetName() const override { return "Begin recording"; }
};

/**
 * End command recording command
 */
class CRenderCommandEndRecording : public IRenderCommand
{
public:
	virtual void Execute(CRenderCommandList* InCmdList) override;

	virtual std::string GetName() const override { return "End recording"; }
};

/**
 * Begin render pass command
 */
class CRenderCommandBeginRenderPass : public IRenderCommand
{
public:
	CRenderCommandBeginRenderPass(const std::array<float, 4>& InClearColor) 
		: ClearColor(InClearColor) {}

	virtual void Execute(CRenderCommandList* InCmdList) override;

	virtual std::string GetName() const override { return "Begin render pass"; }
private:
	std::array<float, 4> ClearColor;
};

/**
 * End render pass command
 */
class CRenderCommandEndRenderPass : public IRenderCommand
{
public:
	virtual void Execute(CRenderCommandList* InCmdList) override;

	virtual std::string GetName() const override { return "End render pass"; }
};

/** 
 * Bind graphics pipeline command
 */
class CRenderCommandBindGraphicsPipeline : public IRenderCommand
{
public:
	CRenderCommandBindGraphicsPipeline(IRenderSystemGraphicsPipeline* InGraphicsPipeline)
		: GraphicsPipeline(InGraphicsPipeline) {}

	virtual void Execute(CRenderCommandList* InCmdList) override;

	virtual std::string GetName() const override { return "Bind graphics pipeline"; }
private:
	IRenderSystemGraphicsPipeline* GraphicsPipeline;
};

/**
 * Bind vertex buffers command
 */
class CRenderCommandBindVertexBuffers : public IRenderCommand
{
public:
	CRenderCommandBindVertexBuffers(CRenderSystemBuffer* InBuffer) :
		VertexBuffers({ InBuffer }) {}

	CRenderCommandBindVertexBuffers(const std::vector<CRenderSystemBuffer*>& InBuffers) :
		VertexBuffers(InBuffers) {}

	virtual void Execute(CRenderCommandList* InCmdList) override;

	virtual std::string GetName() const override { return "Bind vertex buffers"; }
private:
	std::vector<CRenderSystemBuffer*> VertexBuffers;
};

/**
 * Bind index buffer command
 */
class CRenderCommandBindIndexBuffer : public IRenderCommand
{
public:
	CRenderCommandBindIndexBuffer(CRenderSystemBuffer* InBuffer,
		const uint64_t& InOffset,
		const EIndexFormat& InIndexFormat) :
		IndexBuffer(InBuffer), Offset(InOffset), IndexFormat(InIndexFormat) {}

	virtual void Execute(CRenderCommandList* InCmdList) override;

	virtual std::string GetName() const override { return "Bind index buffer"; }
private:
	CRenderSystemBuffer* IndexBuffer;
	uint64_t Offset;
	EIndexFormat IndexFormat;
};

/**
 * Draw command (non-indexed)
 */
class CRenderCommandDraw : public IRenderCommand
{
public:
	CRenderCommandDraw(const uint32_t& InVertexCount, const uint32_t& InInstanceCount,
		const uint32_t& InFirstVertex, const uint32_t& InFirstInstance) :
		VertexCount(InVertexCount), InstanceCount(InInstanceCount),
		FirstVertex(InFirstVertex), FirstInstance(InFirstInstance) {}

	virtual void Execute(CRenderCommandList* InCmdList) override;

	virtual std::string GetName() const override { return "Draw"; }
private:
	uint32_t VertexCount;
	uint32_t InstanceCount;
	uint32_t FirstVertex;
	uint32_t FirstInstance;
};

/**
 * Draw command (indexed)
 */
class CRenderCommandDrawIndexed : public IRenderCommand
{
public:
	CRenderCommandDrawIndexed(const uint32_t& InIndexCount, const uint32_t& InInstanceCount,
		const uint32_t& InFirstIndex, const int32_t& InVertexOffset, const uint32_t& InFirstInstance) :
		IndexCount(InIndexCount), InstanceCount(InInstanceCount),
		FirstIndex(InFirstIndex), VertexOffset(InVertexOffset), FirstInstance(InFirstInstance) {}

	virtual void Execute(CRenderCommandList* InCmdList) override;

	virtual std::string GetName() const override { return "Draw Indexed"; }
private:
	uint32_t IndexCount;
	uint32_t InstanceCount;
	uint32_t FirstIndex;
	int32_t VertexOffset;
	uint32_t FirstInstance;
};

/**
 * Set shader attribute resource command
 */
class CRenderCommandSetShaderAttributeResource : public IRenderCommand
{
public:
	CRenderCommandSetShaderAttributeResource(IShaderAttributesManager* InShaderAttributesManager,
		EShaderStage InShaderStage, const std::string& InName, IDeviceResource* InResource) :
		ShaderAttributesManager(InShaderAttributesManager), Stage(InShaderStage),
		Name(InName), Resource(InResource) {}

	virtual void Execute(CRenderCommandList* InCmdList) override;

	virtual std::string GetName() const override { return "Set shader attribute resource"; }
private:
	IShaderAttributesManager* ShaderAttributesManager;
	EShaderStage Stage;
	std::string Name;
	IDeviceResource* Resource;
};

/**
 * Update a uniform buffer
 */
class CRenderCommandUpdateUniformBuffer : public IRenderCommand
{
public:
	CRenderCommandUpdateUniformBuffer(IRenderSystemUniformBuffer* InUniformBuffer,
		void* InData, const size_t& InDataSize) : UniformBuffer(InUniformBuffer),
		Data(InData), DataSize(InDataSize) {}

	virtual void Execute(CRenderCommandList* InCmdList) override;

	virtual std::string GetName() const override { return "Update uniform buffer"; }
private:
	IRenderSystemUniformBufferPtr UniformBuffer;
	void* Data;
	size_t DataSize;
};

/**
 * Shader attribute manager
 */
class CRenderCommandBindShaderAttributeManager : public IRenderCommand
{
public:
	CRenderCommandBindShaderAttributeManager(
		const std::shared_ptr<IShaderAttributesManager>& InShaderAttributeManager)
		: ShaderAttributesManager(InShaderAttributeManager) {}

	virtual void Execute(CRenderCommandList* InCmdList) override;

	virtual std::string GetName() const override { return "Bind shader attribute manager"; }
private:
	std::shared_ptr<IShaderAttributesManager> ShaderAttributesManager;
};