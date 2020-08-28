#include "EntityProperties.h"
#include "ImGui/ImGui.h"

namespace ZE::Editor
{

CEntityProperties::CEntityProperties(CMapEditor& InMapEditor) : MapEditor(InMapEditor) {}

void CEntityProperties::Draw()
{
	if(!ImGui::Begin("Entity Properties"))
	{
		ImGui::End();
		return;
	}

	ImGui::End();
}

}