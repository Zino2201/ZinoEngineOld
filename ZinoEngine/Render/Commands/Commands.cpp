#include "Commands.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Core/Engine.h"
#include "Render/Commands/RenderCommandContext.h"

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
	CommandMutex.lock();
	auto& Command = Commands.front();
	if(Command)
		Command->Execute(this);
	Commands.pop();
	CommandMutex.unlock();
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