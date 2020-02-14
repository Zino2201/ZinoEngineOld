#pragma once

#include "Render/RenderCore.h"
#include <queue>

/**
 * Commands classes
 */

class CRenderCommandList;
class IRenderCommandContext;
class IRenderSystemGraphicsPipeline;
class CRenderSystemBuffer;
class IShaderAttributesManager;

class ICommand
{
public:
	virtual std::string GetName() const = 0;
};

/**
 * Base render command
 */
class IRenderCommand : public ICommand
{
public:
	virtual ~IRenderCommand() = default;

	virtual void Execute(CRenderCommandList* InCmdList) = 0;
	
	virtual std::string GetName() const override = 0;
};

/**
 * Class for lambdas
 */
template<typename LambdaType>
class CRenderCommandLambda : public IRenderCommand
{
public:
	CRenderCommandLambda(LambdaType&& InLambda) :
		Lambda(InLambda) {}

	virtual void Execute(CRenderCommandList* InCmdList) override
	{
		Lambda(InCmdList);
	}

	virtual std::string GetName() const override { return "Lambda"; }
private:
	LambdaType Lambda;
};

/**
 * Render command list
 */
class CRenderCommandList
{
public:
	CRenderCommandList();
	~CRenderCommandList();

	void Initialize();

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
	 * Enqueue a lambda
	 */
	template<typename Lambda>
	void Enqueue(Lambda&& InLambda)
	{
		Commands.push(std::make_unique<CRenderCommandLambda<Lambda>>(std::forward<Lambda>(InLambda)));
	}

	/**
	 * Execute command in front of the queue
	 */
	void ExecuteFrontCommand();

	void Flush();

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
	virtual void BindGraphicsPipeline(IRenderSystemGraphicsPipeline* InGraphicsPipeline) = 0;

	virtual void BindShaderAttributesManager(
		const std::shared_ptr<IShaderAttributesManager>& InManager) = 0;

	/**
	 * Bind vertex buffers
	 */
	virtual void BindVertexBuffers(const std::vector<CRenderSystemBuffer*>& InVertexBuffers) = 0;

	/**
	 * Bind index buffer
	 */
	virtual void BindIndexBuffer(CRenderSystemBuffer* InIndexBuffer,
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