#pragma once

#include "EngineCore.h"

namespace ZE::Editor
{

class ASSETEXPLORER_API CAssetExplorer
{
public:
	CAssetExplorer();

	void Draw();
private:
	void DrawAssetHierarchy();
	void DrawAssetList();
private:
	float MaxHierarchyWidth;
};

}