#include "Commands.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Core/Engine.h"

CRenderCommandList::CRenderCommandList() 
{
}

CRenderCommandList::~CRenderCommandList() { }

void CRenderCommandList::Initialize()
{
	CommandContext = g_Engine->GetRenderSystem()->GetRenderCommandContext();
}

void CRenderCommandList::ExecuteFrontCommand()
{
	const std::unique_ptr<IRenderCommand>& Command = Commands.front();
	if(Command)
		Command->Execute(this);
	Commands.pop();
}

void CRenderCommandList::Flush()
{
	/** Execute all commands */
	while (!Commands.empty())
	{
		ExecuteFrontCommand();
	}

	ClearQueue();
}

void CRenderCommandList::ClearQueue()
{
	Commands = {};
}