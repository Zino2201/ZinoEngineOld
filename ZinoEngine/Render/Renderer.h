#pragma once

#include "RenderCore.h"

class CRenderCommandList;

/**
 * Main renderer
 */
class CRenderer
{
public:
	CRenderer();

	CRenderCommandList* GetMainCommandList() const { return MainCommandList.get(); }
private:
	std::unique_ptr<CRenderCommandList> MainCommandList;
};