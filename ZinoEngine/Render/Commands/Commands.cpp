#include "Commands.h"
#include "Render/RenderSystem.h"
#include "Core/Engine.h"

CRenderCommandList::CRenderCommandList() 
{
	/** Ask the render system to create a command context */
	CommandContext = std::unique_ptr<IRenderCommandContext>(
		CEngine::Get().GetRenderSystem()->CreateCommandContext());
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