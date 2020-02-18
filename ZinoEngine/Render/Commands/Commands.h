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
class IRenderSystemUniformBuffer;
class CRenderSystemTextureView;

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