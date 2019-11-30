#pragma once

#include "Render/RenderCore.h"
#include <queue>

/**
 * Commands classes
 */

class CRenderCommandList;
class IRenderCommandContext;
class IGraphicsPipeline;
class IBuffer;

/**
 * Base render command
 */
class IRenderCommand
{
public:
	virtual ~IRenderCommand() = default;

	virtual void Execute(CRenderCommandList* InCmdList) = 0;
};

/**
 * Render command list
 */
class CRenderCommandList
{
public:
	CRenderCommandList();
	~CRenderCommandList();

	/**
	 * Enqueue a command
	 */
	template<typename Cmd, typename... Args>
	Cmd* Enqueue(Args&&... InArgs)
	{
		static_assert(std::is_convertible<Cmd*, IRenderCommand*>::value,
			"Command class should derive from IRenderCommand");
		Commands.push(std::make_unique<Cmd>(InArgs...));
		return static_cast<Cmd*>(Commands.back().get());
	}

	/**
	 * Execute command in front of the queue
	 */
	void ExecuteFrontCommand();

	void ClearQueue();

	IRenderCommandContext* GetCommandContext() const { return CommandContext; }
	const std::size_t GetCommandsCount() { return Commands.size(); }
private:
	std::queue<std::unique_ptr<IRenderCommand>> Commands;
	IRenderCommandContext* CommandContext;
};

/**
 * Render command context
 *
 * Used for rendering
 */
class IRenderCommandContext
{
public:
	virtual ~IRenderCommandContext() = default;

	/**
	 * Begin command recording 
	 */
	virtual void Begin() = 0;

	/** 
	 * End command recording 
	 */
	virtual void End() = 0;

	/**
	 * Begin render pass
	 */
	virtual void BeginRenderPass(const std::array<float, 4>& InClearColor) = 0;

	/**
	 * End render pass
	 */
	virtual void EndRenderPass() = 0;

	/**
	 * Bind graphics pipeline
	 */
	virtual void BindGraphicsPipeline(IGraphicsPipeline* InGraphicsPipeline) = 0;

	/**
	 * Bind vertex buffers
	 */
	virtual void BindVertexBuffers(const std::vector<std::shared_ptr<IBuffer>>& InVertexBuffers) = 0;

	/**
	 * Bind index buffer
	 */
	virtual void BindIndexBuffer(const std::shared_ptr<IBuffer>& InIndexBuffer,
		const uint64_t& InOffset,
		const EIndexFormat& InIndexFormat) = 0;

	/**
	 * Draw
	 */
	virtual void Draw(const uint32_t& InVertexCount, const uint32_t& InInstanceCount,
		const uint32_t& InFirstVertex, const uint32_t& InFirstInstance) = 0;

	virtual void DrawIndexed(const uint32_t& InIndexCount, const uint32_t& InInstanceCount,
		const uint32_t& InFirstIndex, const int32_t& InVertexOffset, const uint32_t& InFirstInstance) = 0;
};