#include "Editor/MapEditor.h"
#include "Editor/EntityList.h"
#include "Editor/EntityProperties.h"

namespace ze::editor
{

CMapEditor::CMapEditor()
{
	EntityList = std::make_unique<CEntityList>(*this);
	EntityProperties = std::make_unique<CEntityProperties>(*this);
}

CMapEditor::~CMapEditor() = default;

void CMapEditor::Draw(const ImGuiID& InDockspaceID)
{
	EntityList->Draw();
	EntityProperties->Draw();
}

}