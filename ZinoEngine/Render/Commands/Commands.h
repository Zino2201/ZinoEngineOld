#pragma once

#include "Render/RenderCore.h"
#include <queue>

/**
 * Commands classes
 */

class CRenderCommandList;
class IRenderCommandContext;

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
		Commands.emplace_back(std::make_unique<Cmd>(InArgs...));
		return static_cast<Cmd*>(Commands.back().get());
	}

	/**
	 * Execute all commands and flush the queue
	 */
	void ExecuteAndFlush();
private:
	std::vector<std::unique_ptr<IRenderCommand>> Commands;
	std::unique_ptr<IRenderCommandContext> CommandContext;
};

/**
 * Render command context
 */
class IRenderCommandContext
{
public:
	virtual ~IRenderCommandContext() = default;

	/**
	 * Begin render pass
	 */
	virtual void BeginRenderPass(const std::array<float, 4>& InClearColor) = 0;
};