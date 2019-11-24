#pragma once

#include "Commands.h"

/**
 * Begin render pass command
 */
class CRenderCommandBeginRenderPass : public IRenderCommand
{
public:
	CRenderCommandBeginRenderPass() {}

	virtual void Execute(CRenderCommandList* InCmdList) override
	{
		
	}
};