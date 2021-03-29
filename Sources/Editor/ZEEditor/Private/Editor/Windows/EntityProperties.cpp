#include "Editor/Windows/EntityProperties.h"
#include <QLabel>

namespace ze::editor
{

EntityProperties::EntityProperties()
{
	setWindowTitle("Entity Properties");

	QLabel* test = new QLabel(this);
	test->setText("Entity Properties");
}

}