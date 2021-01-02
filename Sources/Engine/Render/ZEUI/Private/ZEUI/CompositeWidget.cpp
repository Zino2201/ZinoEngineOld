#include "ZEUI/CompositeWidget.h"

namespace ze::ui
{

void CompositeWidget::paint()
{
	if(content_.is_valid())
		content_.get().paint();
}

}