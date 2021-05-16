#include "Editor/Windows/Assets/Texture/TextureEditor.h"
#include "Assets/Asset.h"
#include <imgui_internal.h>
#include "Engine/Assets/Texture.h"
#include "Editor/Assets/TextureCooker.h"

namespace ze::editor
{

TextureEditor::TextureEditor(Asset* in_asset, 
	const assetmanager::AssetRequestPtr& in_request_handle)
	: Window(in_asset->get_path().stem().string(),
		WindowFlagBits::Transient | WindowFlagBits::DockToMainDockSpaceOnce,
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse),
		asset_request(in_request_handle),
		texture(static_cast<Texture*>(in_asset)),
	properties_editor(reflection::Class::get<Texture>(), in_asset), 
	zoom(1.f), 
	current_miplevel(0), 
	should_update_resource(false), 
	should_update_mipmaps(false)
{
	properties_editor.bind_to_value_changed("use_mipmaps", std::bind(&TextureEditor::on_mipmap_changed, this, std::placeholders::_1));

	build_views();
}

TextureEditor::~TextureEditor() {}

void TextureEditor::build_views()
{
	views.clear();
	views.reserve(texture->get_platform_data().get_mip_count());
	for(size_t mip = 0; mip < texture->get_platform_data().get_mip_count(); ++mip)
	{
		views.emplace_back(gfx::Device::get().create_texture_view(gfx::TextureViewInfo::make_2d_view(
			texture->get_texture(), texture->get_platform_data().get_format(),
			gfx::TextureSubresourceRange(gfx::TextureAspectFlagBits::Color, mip, 1, 0, 1))).second);
	}
}

void TextureEditor::draw()
{
	ImGui::Columns(2, nullptr, false);

	/** Used for deferred resource updating */
	if(should_update_mipmaps)
	{
		AssetCooker::cook_asset(texture);
		texture->update_resource();
		should_update_mipmaps = false;
		should_update_resource = false;
		current_miplevel = 0;
		build_views();
	}

	if(should_update_resource)
	{
		AssetCooker::cook_asset(texture);
		texture->update_resource();
		should_update_resource = false;
		build_views();
	}

	const ImVec2 texture_size(ImGui::GetContentRegionAvail());
	ImVec2 texture_quad_size(texture->get_width() * zoom, texture->get_height() * zoom);
	if(views[current_miplevel])
	{
		ImRect bb(ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos() + texture_size);
		ImGui::GetWindowDrawList()->AddRectFilled(bb.Min, bb.Max,
			IM_COL32(0, 0, 0, 255));
		ImVec2 cursor_pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(texture_size / 2 - texture_quad_size / 2);
		ImGui::Image((void*) &views[current_miplevel].get(), texture_quad_size);
		if (ImGui::IsItemHovered() && ImGui::GetIO().MouseWheel != 0.f)
		{
			zoom = std::clamp(zoom + (ImGui::GetIO().MouseWheel * (ImGui::GetIO().DeltaTime * 50.f)), 0.1f, 5.f);
		}
		ImGui::SetCursorPos(cursor_pos + ImVec2(5, 5));
	}

	/** "HUD" display onto the image displaying informations */
	ImGui::BeginGroup();
	ImGui::Text("Type: %s", std::to_string(texture->get_type()).c_str());
	ImGui::Text("GPU Format: %s", to_string_pretty(texture->get_platform_data().get_format()).c_str());
	ImGui::Text("Res: %dx%dx1 (Mip %d/%d)",
		texture->get_platform_data().get_mip(current_miplevel).width,
		texture->get_platform_data().get_mip(current_miplevel).height,
		current_miplevel,
		texture->get_platform_data().get_mip_count() - 1);
	ImGui::Text("VRAM Size (Mip %d): %f KiB",
		current_miplevel,
		texture->get_platform_data().get_mip(current_miplevel).data.size() / 1048576.f);

	{
		std::string zoom_txt = fmt::format("Zoom: {}%", static_cast<int>(zoom * 100));
		ImGui::TextUnformatted(zoom_txt.c_str());
	}

	ImGui::EndGroup();

	ImGui::NextColumn();
	
	/** Parameters */
	ImGui::BeginChild("Properties", ImGui::GetContentRegionAvail(), true);
	{
		ImGui::TextUnformatted("Current selected miplevel");
		ImGui::SameLine();
		ImGui::SliderInt("", &current_miplevel, 0, texture->get_platform_data().get_mip_count() - 1, "%d", 
			ImGuiSliderFlags_AlwaysClamp);
		ImGui::Separator();

		ImGui::TextUnformatted("Parameters");
		if(properties_editor.draw())
		{ // fo enlever
			should_update_resource = true;
		}
	}
	ImGui::EndChild();

	ImGui::Columns(1);
}

void TextureEditor::on_mipmap_changed(void* in_enabled)
{
	should_update_mipmaps = true;
}

}

