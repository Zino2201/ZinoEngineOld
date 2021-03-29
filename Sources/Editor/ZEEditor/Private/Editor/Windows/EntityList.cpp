#include "Editor/Windows/EntityList.h"
#include <QLabel>

namespace ze::editor
{

EntityList::EntityList()
{
	setWindowTitle("Entity List");

	QLabel* test = new QLabel(this);
	test->setText("Entity List");
}

}