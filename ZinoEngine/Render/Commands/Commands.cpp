#include "Commands.h"
#include "Render/RenderSystem.h"
#include "Core/Engine.h"

CRenderCommandList::CRenderCommandList() 
{
	CommandContext = CEngine::Get().GetRenderSystem()->GetRenderCommandContext();
}

CRenderCommandList::~CRenderCommandList() {}

void CRenderCommandList::ExecuteAndFlush()
{
	for (const std::unique_ptr<IRenderCommand>& Command : Commands)
	{
		Command->Execute(this);
	}

	Commands.clear();
}