#include "Editor/Windows/AssetExplorer.h"
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QSplitter>
#include <QTreeWidget>
#include <QStandardItemModel>
#include <QPushButton>
#include <QListWidget>
#include "Reflection/Class.h"

namespace ze::editor
{

AssetExplorer::AssetExplorer()
	: current_directory("Assets")
{
	setWindowTitle("Asset Explorer");

	QHBoxLayout* hbox = new QHBoxLayout;
	hbox->setContentsMargins(QMargins(0, 0, 0, 0));
	QSplitter* splitter = new QSplitter;
	hbox->addWidget(splitter, 0);
	
	setLayout(hbox);

	/** Project hierarchy */
	{
		project_hierarchy_tree = new QTreeWidget(this);
		project_hierarchy_tree->setHeaderHidden(true);
		project_hierarchy_tree->setFrameShape(QFrame::NoFrame);
		project_hierarchy_tree->setSelectionMode(QAbstractItemView::SingleSelection);
		project_hierarchy_tree->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

		QTreeWidgetItem* item = new QTreeWidgetItem;
		item->setText(0, "Assets");
		item->setIcon(0, QIcon(":Icons/icons8-folder-64.png"));
		item->setData(0, Qt::UserRole, QVariant::fromValue(QString::fromStdString("Assets")));
		project_hierarchy_tree->insertTopLevelItem(0, item);

		update_project_hierarchy(item, project_hierarchy_tree->rootIndex(), "Assets");
		generate_directories_map();
		project_hierarchy_tree->selectionModel()->select(directory_to_project_hierarchy_index["Assets"], QItemSelectionModel::Select);

		project_hierarchy_tree->expandAll();

		splitter->addWidget(project_hierarchy_tree);
		splitter->setStretchFactor(0, 1);
		connect(project_hierarchy_tree, &QTreeWidget::itemSelectionChanged, this, &AssetExplorer::on_directory_selected);
	}

	/** Asset list */
	{
		QScrollArea* scroll_area = new QScrollArea(this);
		scroll_area->setWidgetResizable(true);
		scroll_area->setFrameShape(QFrame::NoFrame);
		scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);

		QWidget* scroll_container = new QWidget;
		scroll_area->setWidget(scroll_container);

		QVBoxLayout* vbox_layout = new QVBoxLayout;
		vbox_layout->setContentsMargins(QMargins(0, 0, 0, 0));
		asset_list = new QListWidget;
		asset_list->setFlow(QListView::LeftToRight);
		asset_list->setWrapping(true);
		asset_list->setResizeMode(QListView::Adjust);
		asset_list->setViewMode(QListView::IconMode);
		asset_list->setIconSize(QSize(64, 64));
		asset_list->setUniformItemSizes(true);
		asset_list->setLayoutMode(QListView::Batched);
		asset_list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
		asset_list->setSelectionMode(QAbstractItemView::ExtendedSelection);
		asset_list->setMovement(QListView::Static);
		asset_list->setWordWrap(true);
		asset_list->setTextElideMode(Qt::ElideNone);
		connect(asset_list, &QListWidget::itemDoubleClicked, this, &AssetExplorer::on_item_double_clicked);

		vbox_layout->addWidget(asset_list);

		scroll_container->setLayout(vbox_layout);
		
		update_asset_list();

		splitter->addWidget(scroll_area);
		splitter->setStretchFactor(1, 5);
	}
}

void AssetExplorer::update_project_hierarchy(QTreeWidgetItem* in_item, const QModelIndex& in_index, const std::filesystem::path& in_root)
{
	for(const auto& subdir : assetdatabase::get_subdirectories(in_root))
	{
		QTreeWidgetItem* item = new QTreeWidgetItem;
		item->setText(0, QString::fromStdString(subdir.string()));
		item->setIcon(0, QIcon(":Icons/icons8-folder-64.png"));
		item->setData(0, Qt::UserRole,
			QVariant::fromValue(QString::fromStdString(std::filesystem::path(in_root / subdir).string())));
		in_item->addChild(item);

		directory_to_project_hierarchy_index.insert({ std::filesystem::path(in_root / subdir).string(), 
			project_hierarchy_tree->indexBelow(in_index) });
		update_project_hierarchy(item, project_hierarchy_tree->indexBelow(in_index), in_root / subdir);
	}
}

void AssetExplorer::update_asset_list()
{
	asset_list->clear();

	for(const auto& directory : assetdatabase::get_subdirectories(current_directory))
	{
		QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(directory.string()));
		item->setIcon(QIcon(":Icons/icons8-folder-64.png"));
		item->setTextAlignment(Qt::AlignCenter | Qt::AlignBottom);
		item->setData(Qt::UserRole, QVariant::fromValue(AssetListEntry{ true, current_directory / directory, {} }));
		asset_list->addItem(item);
	}

	for(const auto& asset : assetdatabase::get_assets(current_directory))
	{
		QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(asset.name));
		item->setIcon(QIcon(":Icons/icons8-file-64.png"));
		item->setTextAlignment(Qt::AlignCenter | Qt::AlignBottom);
		item->setToolTip(QString::asprintf("Class: %s\nSize: %f KiB\nPath: %s",
			asset.asset_class->get_name().c_str(),
			static_cast<float>(asset.size / 1048576.f),
			asset.path.string().c_str()));
		item->setData(Qt::UserRole, QVariant::fromValue(AssetListEntry{ false, asset.path, asset }));
		asset_list->addItem(item);
	}
}

void AssetExplorer::on_directory_selected()
{
	QTreeWidgetItem* item = project_hierarchy_tree->selectedItems()[0];
	QString directory = qvariant_cast<QString>(item->data(0, Qt::UserRole));
	current_directory = directory.toStdString();
	update_asset_list();	
}

void AssetExplorer::on_item_double_clicked(QListWidgetItem* in_item)
{
	AssetListEntry data = qvariant_cast<AssetListEntry>(in_item->data(Qt::UserRole));
	if(data.is_directory)
	{
		current_directory = data.path;
		project_hierarchy_tree->selectionModel()->select(
			directory_to_project_hierarchy_index[data.path.string()], 
			QItemSelectionModel::ClearAndSelect);
		update_asset_list();
	}
}

void AssetExplorer::generate_directories_map()
{
	directory_to_project_hierarchy_index.clear();

	for(int row = 0; row < project_hierarchy_tree->model()->rowCount(); ++row)
	{
		generate_directory_map(project_hierarchy_tree->model()->index(row, 0), "");
	}
}

void AssetExplorer::generate_directory_map(const QModelIndex& in_index, const std::filesystem::path& in_path)
{
	if(!in_index.isValid())
		return;

	QTreeWidgetItem* item = reinterpret_cast<QTreeWidgetItem*>(in_index.internalPointer());
	auto test = in_path / item->text(0).toStdString();
	directory_to_project_hierarchy_index.insert({ std::filesystem::path(in_path / item->text(0).toStdString()).string(), in_index });

	int rows = project_hierarchy_tree->model()->rowCount(in_index);
	int cols = project_hierarchy_tree->model()->columnCount(in_index);

	for(int i = 0; i < rows; ++i)
	{
		for(int j = 0; j < cols; ++j)
		{
			QModelIndex child = project_hierarchy_tree->model()->index(i, j, in_index);
			QTreeWidgetItem* child_item = reinterpret_cast<QTreeWidgetItem*>(in_index.internalPointer());
			generate_directory_map(child, std::filesystem::path(in_path / child_item->text(0).toStdString()).string());
		}
	}
}

}