#include "Editor/Widgets/AssetExplorer.h"
#include "ZEUI/Primitives/Text.h"
#include "ZEUI/Containers/VerticalContainer.h"
#include "ZEUI/Containers/HorizontalContainer.h"
#include "ZEUI/Containers/ScrollableContainer.h"
#include "ZEFS/Utils.h"
#include "SDFFontGen.h"

namespace ze::editor
{

AssetExplorer::AssetExplorer() : asset_list(nullptr)
{
}

void AssetExplorer::construct()
{
	using namespace ui;

	auto fd = filesystem::read_file_to_vector("Assets/Fonts/Roboto-Thin.ttf", true);
	auto sdf = generate_msdf_texture_from_font(fd, 16);

	std::vector<Font::Glyph> glyphs;
	glyphs.reserve(sdf.glyphs.size());
	for(const auto& glyph : sdf.glyphs)
		glyphs.emplace_back(glyph.character, 
			glyph.advance, 
			Font::Glyph::Bounds { glyph.bounds.l, glyph.bounds.b, glyph.bounds.r, glyph.bounds.t }, 
			glyph.atlas_rect);
	test = std::make_unique<Font>(fd, sdf.width, sdf.height, sdf.raw_data, 2.0,
		glyphs,
		sdf.em_size,
		sdf.space_advance,
		sdf.tab_advance);

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
				make_widget<Text>()
				->text("COUCOU CA VA ?")
				->font(FontInfo(test.get(), 32))
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