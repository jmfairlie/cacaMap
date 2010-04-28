#include "myderivedmap.h"

myDerivedMap::myDerivedMap(QWidget* parent):cacaMap(parent)
{
	
}
void myDerivedMap::paintEvent(QPaintEvent *e)
{
	cacaMap::paintEvent(e);
	QPainter p(this);
	p.drawRect(100,100,10,10);
}

