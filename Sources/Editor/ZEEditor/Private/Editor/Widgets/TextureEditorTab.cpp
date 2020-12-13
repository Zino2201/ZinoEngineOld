#include "Editor/Widgets/TextureEditorTab.h"
#include "ImGui/ImGui.h"
#include "imgui_internal.h"
#include "Editor/PropertyEditor.h"
#include "Engine/InputSystem.h"
#include "Editor/AssetUtils/AssetUtils.h"

namespace ze::editor
{

TextureEditorTab::TextureEditorTab(Texture* in_texture,
	const std::shared_ptr<assetmanager::AssetRequestHandle>& in_request_handle)
	: texture(in_texture), request_handle(in_request_handle), mip_level(0)
{
}

void TextureEditorTab::draw()
{
	ZE_CHECK(texture && request_handle);

	ImGui::PushID(texture->get_path().string().c_str());
	{
		ui::SImGuiAutoStyleVar WinPadding(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		if (!ImGui::BeginChild("TabTextureEditor", ImVec2(0, 0), false, ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar))
		{
			ImGui::EndChild();
			ImGui::PopID();
			return;
		}
	}

	if(ze::input::is_key_held(SDL_SCANCODE_LCTRL)
		&& ze::input::is_key_pressed(SDL_SCANCODE_S))
	{
		assetutils::save_asset(*texture);
		texture->update_resource();
	}

	ImGui::BeginChild("Texture", ImVec2(800, 500), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	uint32_t max_w = 800;
	uint32_t max_h = 500;

	uint32_t width = std::min<uint32_t>(texture->get_width(), max_w);
	uint32_t height = std::min<uint32_t>(texture->get_height(), max_h);

	ImVec2 cursor_before_img = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2((max_w - width) / 2, (max_h - height) / 2));
	if(texture->is_ready())
	{
		ImGui::Image(const_cast<gfx::ResourceHandle*>(&texture->get_texture_view()), ImVec2(
			width, 
			height));
	}

	ImGui::SetCursorPos(cursor_before_img);
	ImGui::Text("Type: %s", std::to_string(texture->get_type()).c_str());
	ImGui::Text("Format: %s", std::to_string(texture->get_gfx_format()).c_str());
	ImGui::Text("%dx%dx%d (Mip %d/%d)", texture->get_width(), texture->get_height(), 1, 
		mip_level, texture->get_mipmaps().size());
	ImGui::Text("Size in VRAM: %f KiB", texture->get_mipmap(mip_level).get_data().size() / 1048576.f);
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("Properties", ImVec2(800, 500), false, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Dummy(ImVec2(1, 5));
	ImGui::Columns(2);
	const reflection::Class* type = reflection::Class::get<Texture>();

	std::vector<const ze::reflection::Property*> properties;
	properties.reserve(type->get_properties().size());
	for(const auto& property : type->get_properties())
		properties.emplace_back(&property);
	std::sort(properties.begin(), properties.end(), [](const auto& left, const auto&right) {
		return *left > *right;
	});

	for(const auto& property_ptr : properties)
	{
		auto& property = *property_ptr;

		if(property.has_metadata("Visible") || property.has_metadata("Editable"))
		{
			if(PropertyEditor* ed = get_property_edtior(property.get_type()))
			{
				if(!property.has_metadata("Editable"))
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				ImGui::Text("%s", property.get_name().c_str());
				if(ImGui::IsItemHovered())
					ImGui::SetTooltip(property.get_type()->get_name().c_str());
				ImGui::NextColumn();
				std::string unique_label = "##" + property.get_name();
				if(ed->draw(unique_label.c_str(), property.get_value_ptr(texture)))
				{
					/** Make a copy as mipmaps is generated */
					auto data = texture->get_mipmap(0).data;
					texture->generate_mipmaps(data);
					texture->update_resource();
				}

				ImGui::NextColumn();
				if(!property.has_metadata("Editable"))
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}
			}
		}
	}
	ImGui::Columns();

	ImGui::EndChild();

	ImGui::EndChild();
	ImGui::PopID();
}

}