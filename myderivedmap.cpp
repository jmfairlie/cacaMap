#include "myderivedmap.h"

myDerivedMap::myDerivedMap(QWidget* parent):cacaMap(parent)
{
	
}
void myDerivedMap::paintEvent(QPaintEvent *e)
{
	
	//you can draw here whatever you want
	//it will show up over the map
	cacaMap::paintEvent(e);
	QPainter p(this);
        QString m = "You Can Draw Anything you want over the map!";
	p.drawText(rect(),Qt::AlignHCenter|Qt::AlignVCenter,m);
	p.drawEllipse(QPoint(width()/2,height()/2),50,50);
}

