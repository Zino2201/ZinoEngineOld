#pragma once

#include "ZEUI/Container.h"
#include "ZEUI/Padding.h"

namespace ze::ui
{

struct HorizontalContainerItem : public ContainerItem
{
	Padding padding_;

	HorizontalContainerItem* padding(const Padding& in_padding) { padding_ = in_padding; return this; }
};

/**
 * A container that sort child widgets horizontally
 */
class HorizontalContainer : public Container
{
public:
	using Item = HorizontalContainerItem;

	void compute_desired_size(const maths::Vector2f& available_size) override;
	void arrange_children() override;
};

}