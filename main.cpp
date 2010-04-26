#include <qapplication.h>
#include "testwidget.h"

int main (int argc, char **argv)
{
	QApplication a(argc, argv);
	testWidget myWidget;
	myWidget.show();
	return a.exec();
}
