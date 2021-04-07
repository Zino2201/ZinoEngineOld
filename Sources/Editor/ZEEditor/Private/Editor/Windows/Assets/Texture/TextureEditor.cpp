#include "Editor/Windows/Assets/Texture/TextureEditor.h"
#include "Assets/Asset.h"
#include <imgui_internal.h>
#include "Engine/Assets/Texture.h"

namespace ze::editor
{

TextureEditor::TextureEditor(Asset* in_asset, 
	const assetmanager::AssetRequestPtr& in_request_handle)
	: Window(in_asset->get_path().filename().replace_extension("").string(),
		WindowFlagBits::Transient | WindowFlagBits::DockToMainDockSpaceOnce),
		asset(in_asset),
		asset_request(in_request_handle),
		texture(static_cast<Texture*>(in_asset)),
	properties_editor(reflection::Class::get<Texture>(), in_asset)
{
}

void TextureEditor::draw()
{
	ImGui::Columns(2, nullptr, false);

	const ImVec2 texture_size(ImGui::GetContentRegionAvail());
	const ImVec2 texture_quad_size(512, 512);
	{
		ImRect bb(ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos() + texture_size);
		ImGui::GetWindowDrawList()->AddRectFilled(bb.Min, bb.Max,
			IM_COL32(0, 0, 0, 255));
		ImVec2 cursor_pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(texture_size / 2 - texture_quad_size / 2);
		ImVec2 screen_pos = ImGui::GetCursorScreenPos();
		ImGui::Image(0, texture_quad_size);
		if (ImGui::IsItemHovered())
        {
			ImGui::BeginTooltip();
			float region_sz = 32.0f;
			float region_x = ImGui::GetIO().MousePos.x - screen_pos.x - region_sz * 0.5f;
			float region_y = ImGui::GetIO().MousePos.y - screen_pos.y - region_sz * 0.5f;
			float zoom = 4.0f;

			if (region_x < 0.0f)
				region_x = 0.0f;
			else if (region_x > texture_quad_size.x - region_sz) 
				region_x = texture_quad_size.x - region_sz;

			if (region_y < 0.0f) 
				region_y = 0.0f;
			else if (region_y > texture_quad_size.y - region_sz) 
				region_y = texture_quad_size.y - region_sz;

			ImVec2 uv0 = ImVec2((region_x) / texture_quad_size.x, (region_y) / texture_quad_size.y);
			ImVec2 uv1 = ImVec2((region_x + region_sz) / texture_quad_size.x, (region_y + region_sz) / texture_quad_size.y);
			ImGui::Image(0, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 0.5));
			ImGui::EndTooltip();
        }
		ImGui::SetCursorPos(cursor_pos + ImVec2(5, 5));
	}

	/** "HUD" display onto the image displaying informations */
	ImGui::BeginGroup();
	ImGui::Text("Type: %s", std::to_string(texture->get_type()).c_str());
	ImGui::Text("Format: %s", std::to_string(texture->get_gfx_format()).c_str());
	ImGui::Text("%dx%dx1 (Mip 0/%d)",
		texture->get_width(),
		texture->get_height(),
		texture->get_mipmaps().size());
	if(!texture->get_mipmaps().empty())
	{
		ImGui::Text("VRAM Size (Mip %d): %d KiB", 
			0,
			(int) texture->get_mipmap(0).get_data().size() / 1048576.f);
	}
	else
	{
		ImGui::TextColored(ImVec4(1.0f, 0, 0, 1), "ERROR: No mipmap data! Asset may be corrupted!");
	}
	ImGui::EndGroup();

	ImGui::NextColumn();
	
	/** Parameters */
	ImGui::BeginChild("Properties", ImGui::GetContentRegionAvail(), true);
	{
		ImGui::TextUnformatted("Parameters");
		properties_editor.draw();
	}
	ImGui::EndChild();

	ImGui::Columns(1);
}

}