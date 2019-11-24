#include "Commands.h"

CRenderCommandList::CRenderCommandList() {}
CRenderCommandList::~CRenderCommandList() {}

void CRenderCommandList::ExecuteAndFlush()
{
	for (const std::unique_ptr<IRenderCommand>& Command : Commands)
	{
		Command->Execute(this);
	}

	Commands.clear();
}