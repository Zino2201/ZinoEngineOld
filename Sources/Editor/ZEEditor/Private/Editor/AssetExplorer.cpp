#include "Editor/AssetExplorer.h"
#include "Module/Module.h"
#include "ImGui/ImGui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include "AssetDatabase/AssetDatabase.h"
#include "Reflection/Class.h"
#include "StringUtil.h"
#include "Editor/AssetUtils/AssetUtils.h"
#include "ZEFS/Paths.h"
#include "Editor/Assets/AssetActions.h"
#include "Assets/AssetManager.h"
#include "Editor/NotificationSystem.h"
#include "Editor/Assets/AssetFactory.h"

namespace ze::editor
{

CAssetExplorer::CAssetExplorer() : MaxHierarchyWidth(250.f) {}

void CAssetExplorer::Draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.5f, 0.5f));
	if (!ImGui::Begin("Asset Explorer", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		ImGui::PopStyleVar();
		ImGui::End();
		return;
	}
	ImGui::PopStyleVar();

	/** Project Hierarchy */
	if(ImGui::BeginChild("Project Hierarchy", ImVec2(MaxHierarchyWidth, 
		ImGui::GetContentRegionAvail().y)))
		DrawAssetHierarchy();

	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
	ImGui::SameLine();

	/** Asset list */
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
	if (ImGui::BeginChild("Asset List", ImGui::GetContentRegionAvail(),
		false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		DrawAssetList();

	ImGui::EndChild();

	ImGui::End();
}

void CAssetExplorer::SelectDirectory(const std::filesystem::path& InPath)
{
	CurrentDirectory = InPath;
}

void CAssetExplorer::DrawRecurseHierachy(const std::filesystem::path& InPath)
{
	std::vector<std::filesystem::path> Subdirs = ze::assetdatabase::get_subdirectories(InPath);
	
	for (const auto& Subdir : Subdirs)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
			ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
		if(selected_dir == Subdir)
			flags |= ImGuiTreeNodeFlags_Selected;

		if (ImGui::TreeNodeEx(Subdir.string().c_str(), flags))
		{
			if (ImGui::IsItemClicked())
			{
				selected_dir = Subdir;
				SelectDirectory(InPath / Subdir);
			}

			DrawRecurseHierachy(InPath / Subdir);
			ImGui::TreePop();
		}
	}
}

void CAssetExplorer::DrawAssetHierarchy()
{
	ImGui::Dummy(ImVec2(3, 5));

	ImGui::Dummy(ImVec2(2, 0));
	ImGui::SameLine();
	if (ImGui::Button("Import", ImVec2(75, 25)))
	{
		ze::editor::assetutils::import_assets_dialog(
			ze::filesystem::get_current_working_dir() / CurrentDirectory, CurrentDirectory);
	}
	
	ImGui::Separator();
	if (ImGui::TreeNodeEx("Game", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		if (ImGui::IsItemClicked())
		{
			SelectDirectory("");
		}

		DrawRecurseHierachy("");
		ImGui::TreePop();
	}
}

void CAssetExplorer::DrawAssetList()
{
	auto Tokens = ze::stringutil::split(CurrentDirectory.string(),
		std::filesystem::path::preferred_separator);
	
	if (Tokens.empty())
		ImGui::Dummy(ImGui::CalcItemSize(ImVec2(50, 25), 50, 25));

	std::filesystem::path Parent = "";
	for (const auto& Token : Tokens)
	{
		ImGui::SameLine();
		ImGui::Button(Token.c_str(), ImVec2(50, 25));
		if (ImGui::IsItemClicked())
			SelectDirectory(Parent / Token);
		ImGui::SameLine();
		Parent /= Token;
		ImGui::Text("/");
	}
	ImGui::Separator();
	
	char Search[256] = "";
	ImGui::InputTextWithHint("Search", "Search...", Search, 256,
		ImGuiInputTextFlags_CallbackAlways, 
		[](ImGuiInputTextCallbackData* InData) -> int
		{
			return 0;
		},
		this);
	ImGui::Dummy(ImVec2(0, 5));

	ImGui::BeginChild("ASL_ScrollArea", ImGui::GetContentRegionAvail());

	if (ImGui::BeginPopupContextWindow())
	{
		if(ImGui::BeginCombo("##AssetCreateNew", "Create new"))
		{
			const auto& factories = get_factories();
			bool nc = true;
			for(const auto& factory : factories)
			{
				if(!factory->can_instantiated() || !factory->get_supported_class())
					continue;

				nc = false;

				if(ImGui::Selectable(factory->get_supported_class()->get_name().c_str()))
				{
					OwnerPtr<Asset> asset = factory->instantiate();
					assetutils::save_asset(*asset, ze::filesystem::get_current_working_dir() / CurrentDirectory, "NewAssetInst");
					delete asset;
					assetdatabase::scan("", assetdatabase::AssetScanMode::Sync);
				}
			}

			if(nc)
				ImGui::Selectable("Nothing to create :(");

			ImGui::EndCombo();
		}

		ImGui::EndPopup();
	}


	/** Display subdirectories */
	for (const auto& Subdir : ze::assetdatabase::get_subdirectories(CurrentDirectory))
	{
		ImVec2 Cursor = ImGui::GetCursorPos();
		std::string ID = "##" + Subdir.string();
		ImGui::Selectable(ID.c_str(), false, ImGuiSelectableFlags_None, ImVec2(75, 100));
		if (ImGui::IsItemHovered() && 
			ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			SelectDirectory(CurrentDirectory / Subdir);
		}

		ImGui::SetCursorPos(Cursor);

		ImGui::BeginChild(Subdir.string().c_str(), ImVec2(75, 100), false, ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoInputs);

		ImVec2 ThumbSize = ImVec2(50, 50);
		ImGui::SetCursorPos((ImGui::GetWindowSize() - ThumbSize) * 0.5f);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 15.f);

		ImGui::Image(0, ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 1, 1));
		
		ImGui::PushTextWrapPos(75.f);
		ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 
			ImGui::CalcTextSize(Subdir.string().c_str()).x) * 0.5f);
		ImGui::TextUnformatted(Subdir.string().c_str());
		ImGui::PopTextWrapPos();

		ImGui::EndChild();
		ImGui::SameLine();
	}

	/** Display assets */
	for (const auto& Asset : ze::assetdatabase::get_assets(CurrentDirectory))
	{
		ImVec2 Cursor = ImGui::GetCursorPos();
		std::string ID = "##" + Asset.name;
		ImGui::Selectable(ID.c_str(), false, ImGuiSelectableFlags_None, ImVec2(75, 100));
		if (ImGui::IsItemHovered())
		{
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if(AssetActions* actions = get_actions_for(Asset.asset_class))
				{
					auto request = assetmanager::load_asset_sync(Asset.path);
					actions->open_editor(request.first, request.second);
				}
				else
                {
				    notification_add(NotificationType::Error, fmt::format("Failed to open asset {}: No asset actions found for this asset type",
                        Asset.path.string()));
                }
			}

			ImGui::BeginTooltip();
			ImGui::Text("Type: %s\nFull path: %s\nSize: %f Mb",
				Asset.asset_class->get_name().c_str(),
				Asset.path.string().c_str(),
				static_cast<float>(Asset.size / 1048576.f));
			ImGui::Separator();
			ImGui::Text("Last modified on ");
			ImGui::Separator();
			ImGui::Text("ZE %d.%d.%d", Asset.engine_ver.major,
				Asset.engine_ver.minor,
				Asset.engine_ver.patch);
			ImGui::EndTooltip();
		}
		ImGui::SetCursorPos(Cursor);
		if (ImGui::BeginChild(Asset.name.c_str(), ImVec2(75, 100), false, ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoInputs))
		{
			ImVec2 ThumbSize = ImVec2(50, 50);
			ImGui::SetCursorPos((ImGui::GetWindowSize() - ThumbSize) * 0.5f);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 15.f);

			ImGui::Image(0, ThumbSize);
			
			ImGui::PushTextWrapPos(75.f);
			ImGui::SetCursorPosX((ImGui::GetWindowSize().x -
				ImGui::CalcTextSize(Asset.name.c_str()).x) * 0.5f);
			ImGui::TextUnformatted(Asset.name.c_str());
			ImGui::PopTextWrapPos();
		}
		ImGui::EndChild();
		ImGui::SameLine();
	}

	ImGui::EndChild();
}

}