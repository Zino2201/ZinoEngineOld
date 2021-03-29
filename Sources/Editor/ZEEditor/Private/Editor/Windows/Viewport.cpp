#include "Editor/Windows/Viewport.h"
#include <QLabel>

namespace ze::editor
{

Viewport::Viewport()
{
	setWindowTitle("Viewport");

	QLabel* t1 = new QLabel(this);
	t1->setText("Viewport");
}

}