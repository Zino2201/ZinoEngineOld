#pragma once

#include "Commands.h"

class IGraphicsPipeline;
class IBuffer;

/**
 * Begin command recording command
 */
class CRenderCommandBeginRecording : public IRenderCommand
{
public:
	virtual void Execute(CRenderCommandList* InCmdList) override;
};

/**
 * End command recording command
 */
class CRenderCommandEndRecording : public IRenderCommand
{
public:
	virtual void Execute(CRenderCommandList* InCmdList) override;
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
};

/** 
 * Bind graphics pipeline command
 */
class CRenderCommandBindGraphicsPipeline : public IRenderCommand
{
public:
	CRenderCommandBindGraphicsPipeline(const std::shared_ptr<IGraphicsPipeline>& InGraphicsPipeline)
		: GraphicsPipeline(InGraphicsPipeline) {}

	virtual void Execute(CRenderCommandList* InCmdList) override;
private:
	std::shared_ptr<IGraphicsPipeline> GraphicsPipeline;
};

/**
 * Bind vertex buffers command
 */
class CRenderCommandBindVertexBuffers : public IRenderCommand
{
public:
	CRenderCommandBindVertexBuffers(const std::shared_ptr<IBuffer>& InBuffer) :
		VertexBuffers({ InBuffer }) {}

	CRenderCommandBindVertexBuffers(const std::vector<std::shared_ptr<IBuffer>>& InBuffers) :
		VertexBuffers(InBuffers) {}

	virtual void Execute(CRenderCommandList* InCmdList) override;
private:
	std::vector<std::shared_ptr<IBuffer>> VertexBuffers;
};

/**
 * Bind index buffer command
 */
class CRenderCommandBindIndexBuffer : public IRenderCommand
{
public:
	CRenderCommandBindIndexBuffer(const std::shared_ptr<IBuffer>& InBuffer,
		const uint64_t& InOffset,
		const EIndexFormat& InIndexFormat) :
		IndexBuffer(InBuffer), Offset(InOffset), IndexFormat(InIndexFormat) {}

	virtual void Execute(CRenderCommandList* InCmdList) override;
private:
	std::shared_ptr<IBuffer> IndexBuffer;
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
private:
	uint32_t IndexCount;
	uint32_t InstanceCount;
	uint32_t FirstIndex;
	int32_t VertexOffset;
	uint32_t FirstInstance;
};