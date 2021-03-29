#pragma once

#include <QWidget>
#include <QFrame>
#include <QAbstractItemModel>
#include <filesystem>
#include <QModelIndex>
#include "AssetDatabase/AssetDatabase.h"
#include <robin_hood.h>

class QVBoxLayout;
class QTreeWidget;
class QListWidget;
class QTreeWidgetItem;
class QListWidgetItem;

namespace ze::editor
{

class AssetExplorer : public QWidget
{
	Q_OBJECT

public:
	struct AssetListEntry
	{
		bool is_directory;
		std::filesystem::path path;
		assetdatabase::AssetPrimitiveData asset;
	};
	
	AssetExplorer();
private slots:
	void on_directory_selected();
	void on_item_double_clicked(QListWidgetItem* in_item);
private:
	void update_project_hierarchy(QTreeWidgetItem* in_item, const QModelIndex& in_index, const std::filesystem::path& in_root);
	void update_asset_list();
	void generate_directories_map();
	void generate_directory_map(const QModelIndex& in_index, const std::filesystem::path& in_path);
private:
	QTreeWidget* project_hierarchy_tree;
	QListWidget* asset_list;
	std::filesystem::path current_directory;
	robin_hood::unordered_map<std::string, QModelIndex> directory_to_project_hierarchy_index;
};

}

Q_DECLARE_METATYPE(ze::editor::AssetExplorer::AssetListEntry);