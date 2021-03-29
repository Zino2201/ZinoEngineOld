#include "Editor/Widgets/AssetExplorer.h"
#include "ZEUI/Primitives/Text.h"
#include "ZEUI/Containers/VerticalContainer.h"
#include "ZEUI/Containers/HorizontalContainer.h"
#include "ZEUI/Containers/ScrollableContainer.h"
#include "ZEFS/Utils.h"
#include "ZEUI/Primitives/Box.h"
#include "SDFFontGen.h"

namespace ze::editor
{

ZEUIAssetExplorer::ZEUIAssetExplorer() : asset_list(nullptr)
{
}

void ZEUIAssetExplorer::construct()
{
	using namespace ui;

	title("Asset Explorer");

	content()
	[
		make_widget<HorizontalContainer>()
		+ make_item<HorizontalContainer>()
		->content()
		[
			make_widget<VerticalContainer>(project_hierarchy)
			+ make_item<VerticalContainer>()
			->content()
			[
				make_widget<Box>()
				->brush(Brush::make_color(maths::Color::from_hex(0x34495e)))
				->content()
				[
					make_widget<Text>()
					->font(FontInfo(test.get(), 32))
					->text("Coucou je suis dans une box :o")
				]
			]
		]
		+ make_item<HorizontalContainer>()
		->content()
		[
			make_widget<VerticalContainer>(allo)
			//make_widget<HorizontalContainer>(asset_list)
		]
	];

	//for(size_t i = 0; i < 200; ++i)
	//	asset_list->add(&make_item<HorizontalContainer>()->padding(Padding(15.f, 15.f, 15.f, 15.f))->content()[ make_widget<Text>()->text("coucou")]);
	//asset_list->set_arranged_rect(maths::Rect2D({}, 1700));

	//for(size_t i = 0; i < 6; ++i)
	//	project_hierarchy->add(&make_item<VerticalContainer>()->content()[ make_widget<Text>()->text("left")]);
	//project_hierarchy->add(&make_item<VerticalContainer>()->content()[ make_widget<Text>()->text("je suis plus gros")]);
	//project_hierarchy->add(&make_item<VerticalContainer>()->content()[ make_widget<Text>()->text("g")]);
	//
	//for(size_t i = 0; i < 10; ++i)
	//	allo->add(&make_item<VerticalContainer>()->content()[ make_widget<Text>()->text("right")]);
	//allo->add(&make_item<VerticalContainer>()->content()[ make_widget<Text>()->text("right end")]);


	DockableTab::construct();
}

}