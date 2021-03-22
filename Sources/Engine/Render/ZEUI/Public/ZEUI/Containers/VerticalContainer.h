#pragma once

#include "ZEUI/Container.h"
#include "ZEUI/Padding.h"
#include "ZEUI/Attribute.h"

namespace ze::ui
{

struct VerticalContainerItem : public ContainerItem
{
	Padding padding_;
	Attribute<HorizontalAlignMode> halign_mode_;

	VerticalContainerItem* padding(const Padding& in_padding) { padding_ = in_padding; return this; }
	VerticalContainerItem* halign_mode(const HorizontalAlignMode& in_align_mode) { halign_mode_ = in_align_mode; return this; }
};

/**
 * A container that sort child widgets vertically
 */
class VerticalContainer : public Container
{
public:
	using Item = VerticalContainerItem;

	void compute_desired_size(maths::Vector2f available_size) override;
	void arrange_children() override;
};

}