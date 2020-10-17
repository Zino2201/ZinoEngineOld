#include "EntityList.h"
#include "ImGui/ImGui.h"

namespace ze::editor
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