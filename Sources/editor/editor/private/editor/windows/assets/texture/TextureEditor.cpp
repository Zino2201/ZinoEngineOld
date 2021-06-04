#include "editor/windows/assets/texture/TextureEditor.h"
#include "assets/Asset.h"
#include <imgui_internal.h>
#include "engine/assets/Texture.h"
#include "editor/assets/TextureCooker.h"

namespace ze::editor
{

TextureEditor::TextureEditor(Texture* in_texture, 
	const assetmanager::AssetRequestPtr& in_request_handle)
	: AssetEditor(in_texture, in_request_handle,
		in_texture->get_path().stem().string(),
		WindowFlagBits::DockToMainDockSpaceOnce,
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse),
	properties_editor(reflection::Class::get<Texture>(), in_texture), 
	zoom(1.f), 
	current_miplevel(0), 
	should_update_resource(false), 
	should_update_mipmaps(false)
{
	properties_editor.bind_to_value_changed("use_mipmaps", 
		std::bind(&TextureEditor::on_mipmap_changed, this, std::placeholders::_1));

	build_views();
}

TextureEditor::~TextureEditor() {}

void TextureEditor::build_views()
{
	views.clear();
	views.reserve(asset->get_platform_data().get_mip_count());
	for(size_t mip = 0; mip < asset->get_platform_data().get_mip_count(); ++mip)
	{
		views.emplace_back(gfx::Device::get().create_texture_view(gfx::TextureViewInfo::make_2d_view(
			asset->get_texture(), asset->get_platform_data().get_format(),
			gfx::TextureSubresourceRange(gfx::TextureAspectFlagBits::Color, mip, 1, 0, 1))).second);
	}
}

void TextureEditor::draw()
{
	ImGui::Columns(2, nullptr, false);

	/** Used for deferred resource updating */
	if(should_update_mipmaps)
	{
		AssetCooker::cook_asset(asset);
		asset->update_resource();
		should_update_mipmaps = false;
		should_update_resource = false;
		current_miplevel = 0;
		build_views();
	}

	if(should_update_resource)
	{
		AssetCooker::cook_asset(asset);
		asset->update_resource();
		should_update_resource = false;
		build_views();
	}

	const ImVec2 texture_size(ImGui::GetContentRegionAvail());
	ImVec2 texture_quad_size(asset->get_width() * zoom, asset->get_height() * zoom);
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
	ImGui::Text("Type: %s", std::to_string(asset->get_type()).c_str());
	ImGui::Text("GPU Format: %s", to_string_pretty(asset->get_platform_data().get_format()).c_str());
	ImGui::Text("Res: %dx%dx1 (Mip %d/%d)",
		asset->get_platform_data().get_mip(current_miplevel).width,
		asset->get_platform_data().get_mip(current_miplevel).height,
		current_miplevel,
		asset->get_platform_data().get_mip_count() - 1);
	ImGui::Text("VRAM Size (Mip %d): %f KiB",
		current_miplevel,
		asset->get_platform_data().get_mip(current_miplevel).data.size() / 1048576.f);
	{
		std::string zoom_txt = fmt::format("Zoom: {}%", static_cast<int>(zoom * 100));
		ImGui::TextUnformatted(zoom_txt.c_str());
	}

	ImGui::EndGroup();

	ImGui::NextColumn();
	
	/** Parameters */
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));
	ImGui::BeginChild("Properties", ImGui::GetContentRegionAvail(), true);
	{
		ImGui::TextUnformatted("Current selected miplevel");
		ImGui::SameLine();
		ImGui::SliderInt("", &current_miplevel, 0, asset->get_platform_data().get_mip_count() - 1, "%d", 
			ImGuiSliderFlags_AlwaysClamp);
		ImGui::Separator();

		ImGui::TextUnformatted("Parameters");
		if(properties_editor.draw())
		{
			should_update_resource = true;
			mark_as_unsaved();
		}
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();

	ImGui::Columns(1);
}

void TextureEditor::on_mipmap_changed(void* in_enabled)
{
	should_update_mipmaps = true;
}

}

