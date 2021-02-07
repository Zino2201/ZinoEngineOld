#pragma once

#include "ZEUI/Container.h"
#include "ZEUI/Padding.h"

namespace ze::ui
{

struct VerticalContainerItem : public ContainerItem
{
	Padding padding_;

	VerticalContainerItem* padding(const Padding& in_padding) { padding_ = in_padding; return this; }
};

/**
 * A container that sort child widgets vertically
 */
class VerticalContainer : public Container
{
public:
	using Item = VerticalContainerItem;

	void compute_desired_size(const maths::Vector2f& available_size) override;
	void arrange_children() override;
};

}