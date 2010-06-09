#include "myderivedmap.h"
using namespace std;

myDerivedMap::myDerivedMap(QWidget* parent):cacaMap(parent)
{
	cout<<"derived constructor"<<endl;
	timer = new QTimer(this);
	mindistance = 0.025;
	animrate = 0.5;	
	
	hlayout = new QHBoxLayout;

	slider = new QSlider(Qt::Vertical,this);
	slider->setTickPosition(QSlider::TicksBothSides);
	slider->setMaximum(maxZoom);
	slider->setMinimum(minZoom);
	slider->setSliderPosition(zoom);
	connect(slider, SIGNAL(valueChanged(int)),this, SLOT(updateZoom(int)));
	
	hlayout->addWidget(slider);
	hlayout->addStretch();
	setLayout(hlayout);

}

myDerivedMap::~myDerivedMap()
{
	delete slider;
	delete hlayout;
}

/**
Saves the screen coordinates of the last click
This is used for scrolling the map
@see myDerived::mouseMoveEvent()
*/
void myDerivedMap::mousePressEvent(QMouseEvent* e)
{
	mouseAnchor = e->pos();
}

/**
Calculates the length of the mouse drag and
translates it into a new coordinate, map is rerendered
*/
void myDerivedMap::mouseMoveEvent(QMouseEvent* e)
{
	QPoint delta = e->pos()- mouseAnchor;
	mouseAnchor = e->pos();
	longPoint p = myMercator::geoCoordToPixel(geocoords,zoom,tileSize);
	
	p.x-= delta.x();
	p.y-= delta.y();
	geocoords = myMercator::pixelToGeoCoord(p,zoom,tileSize);
	updateContent();
	update();
}

void myDerivedMap::mouseDoubleClickEvent(QMouseEvent* e)
{
	//do the zoom-in animation magic
	if (e->button() == Qt::LeftButton)
	{
		QPoint deltapx = e->pos() - QPoint(width()/2,height()/2);
		longPoint currpospx = myMercator::geoCoordToPixel(geocoords,zoom,tileSize);
		longPoint newpospx;
		newpospx.x = currpospx.x + deltapx.x();
		newpospx.y = currpospx.y + deltapx.y();
		destination = myMercator::pixelToGeoCoord(newpospx,zoom,tileSize);
		connect(timer,SIGNAL(timeout()),this,SLOT(zoomAnim()));
		timer->start(40);
	}
	//do a simple zoom out for now
	else if (e->button() == Qt::RightButton)
	{
		zoomOut();
		slider->setSliderPosition(zoom);
		update();
	}
}

void myDerivedMap::zoomAnim()
{
	float delta = buffzoomrate - 0.5;
	if (delta > mindistance)
	{
		QPointF deltaSpace = destination - geocoords;
		geocoords+=animrate*deltaSpace;
		buffzoomrate-= delta*animrate; 
		updateContent();
	}
	//you are already there
	else
	{
		timer->stop();
		disconnect(timer,SIGNAL(timeout()),this,SLOT(zoomAnim()));
		geocoords = destination;
		buffzoomrate = 1.0;
		zoomIn();
		slider->setSliderPosition(zoom);
	}
	update();
}
void myDerivedMap::updateZoom(int newZoom)
{
	setZoom(newZoom);
	update();
}
void myDerivedMap::paintEvent(QPaintEvent *e)
{
	cacaMap::paintEvent(e);
}
