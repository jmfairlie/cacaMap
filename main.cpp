#include <qapplication.h>
#include "cacamap.h"


int main (int argc, char **argv)
{
	QApplication a(argc, argv);
	cacaMap myMap;
	myMap.show();
	return a.exec();
}
