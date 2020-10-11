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

DEFINE_MODULE(ZE::Module::CDefaultModule, AssetExplorer);

namespace ZE::Editor
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
	std::vector<std::filesystem::path> Subdirs = ZE::AssetDatabase::GetSubDirectories(InPath);
	
	for (const auto& Subdir : Subdirs)
	{
		if (ImGui::TreeNodeEx(Subdir.string().c_str(), ImGuiTreeNodeFlags_DefaultOpen |
			ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth))
		{
			if (ImGui::IsItemClicked())
			{
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
		ZE::Editor::AssetUtils::ImportAssetsDialog(
			ZE::FileSystem::Paths::GetCurrentWorkingDir() / CurrentDirectory, CurrentDirectory);
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
	auto Tokens = ZE::StringUtil::Split(CurrentDirectory.string(),
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

	/** Display subdirectories */
	for (const auto& Subdir : ZE::AssetDatabase::GetSubDirectories(CurrentDirectory))
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
	for (const auto& Asset : ZE::AssetDatabase::GetAssets(CurrentDirectory))
	{
		ImVec2 Cursor = ImGui::GetCursorPos();
		std::string ID = "##" + Asset.Name;
		ImGui::Selectable(ID.c_str(), false, ImGuiSelectableFlags_None, ImVec2(75, 100));
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Type: %s\nFull path: %s\nSize: %f Mb",
				"===class===",
				Asset.Path.string().c_str(),
				static_cast<float>(Asset.Size / 1024.f / 1024.f));
			ImGui::Separator();
			ImGui::Text("Last modified on ");
			ImGui::Separator();
			ImGui::Text("ZE %d.%d.%d", Asset.EngineVer.Major,
				Asset.EngineVer.Minor,
				Asset.EngineVer.Patch);
			ImGui::EndTooltip();
		}
		ImGui::SetCursorPos(Cursor);
		if (ImGui::BeginChild(Asset.Name.c_str(), ImVec2(75, 100), false, ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoInputs))
		{
			ImVec2 ThumbSize = ImVec2(50, 50);
			ImGui::SetCursorPos((ImGui::GetWindowSize() - ThumbSize) * 0.5f);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 15.f);

			ImGui::Image(0, ThumbSize);
			
			ImGui::PushTextWrapPos(75.f);
			ImGui::SetCursorPosX((ImGui::GetWindowSize().x -
				ImGui::CalcTextSize(Asset.Name.c_str()).x) * 0.5f);
			ImGui::Text(Asset.Name.c_str());
			ImGui::PopTextWrapPos();
		}
		ImGui::EndChild();
		ImGui::SameLine();
	}

	ImGui::EndChild();
}

}