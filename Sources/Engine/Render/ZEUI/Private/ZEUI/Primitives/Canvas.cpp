#include "ZEUI/Primitives/Canvas.h"
#include "ZEUI/Primitives/Text.h"
#include "ImGui/ImGui.h"

namespace ze::ui
{

//void Canvas::paint()
//{
//	for(const auto& item_ : items)
//	{
//		CanvasItem* item = static_cast<CanvasItem*>(item_.get());
//		
//		ImVec2 size = ImGui::GetWindowSize();
//
//		/*ImGui::SetCursorPos(ImVec2(
//			 (item->get_anchor().anchor.x * size.x) - (item->get_alignement().x * item->get().get_size().x) + item->get_position().x, 
//			 (item->get_anchor().anchor.y * size.y) - (item->get_alignement().y * item->get().get_size().y) + item->get_position().y));*/
//		item->get().paint();
//	}
//}

}
