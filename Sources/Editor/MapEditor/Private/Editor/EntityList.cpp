#include "EntityList.h"
#include "ImGui/ImGui.h"

namespace ZE::Editor
{

CEntityList::CEntityList(CMapEditor& InMapEditor) : MapEditor(InMapEditor) {}

void CEntityList::Draw()
{
	if(!ImGui::Begin("Entity List"))
	{
		ImGui::End();
		return;
	}

	ImGui::End();
}

}