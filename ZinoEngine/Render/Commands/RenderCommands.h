#pragma once

#include "Commands.h"

class IGraphicsPipeline;

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
	CRenderCommandBeginRenderPass(const std::array<float, 4>& InClearColor) {}

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

class CRenderCommandDraw : public IRenderCommand
{
public:
	CRenderCommandDraw(const uint32_t& InVertexCount, const uint32_t& InInstanceCount,
		const uint32_t& InFirstVertex, const uint32_t& InFirstInstance) :
		VertexCount(InVertexCount), InstanceCount(InInstanceCount),
		FirstVertex(InFirstVertex), FirstInstance(InFirstVertex) {}

	virtual void Execute(CRenderCommandList* InCmdList) override;
private:
	uint32_t VertexCount;
	uint32_t InstanceCount;
	uint32_t FirstVertex;
	uint32_t FirstInstance;
};