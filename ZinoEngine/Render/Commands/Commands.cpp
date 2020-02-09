#include "Commands.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Core/Engine.h"

CRenderCommandList::CRenderCommandList() 
{
	CommandContext = CEngine::Get().GetRenderSystem()->GetRenderCommandContext();
}

CRenderCommandList::~CRenderCommandList() {}

void CRenderCommandList::ExecuteFrontCommand()
{
	const std::unique_ptr<IRenderCommand>& Command = Commands.front();
	Command->Execute(this);
	Commands.pop();
}

void CRenderCommandList::ClearQueue()
{
	Commands = {};
}