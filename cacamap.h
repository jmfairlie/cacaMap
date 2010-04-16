#ifndef CACAMAP_H
#define CACAMAP_H

#include <QtGui>
#include <QtNetwork>
#include <iostream>
#include <vector>

/*
 * The class cant be specified in great detail because I dont know the Qt API yet
 * just the general idea will be described, implementator should add to the  prototype as he deems fit



Class should retrieve the tiles from the specified source (e.g. OSM, google)
and cache them in the hard drive
Class should keep track of which tiles are already on the cache in order to minimize downloads

The cache should have a maximum size (e.g 1, 2MB), when reached tiles not in use should be deleted

The current location should be depicted always in the middle of the widget.
If Widget is bigger than the tiles to be shown or one of the edges is visible 
then tiles should be repeated horizontally and/or vertically as needed

Map should be draggable, when dragging or zooming images that are not cached should be queued for download, and a 'loading' place holder image should be shown in their place
Images that are not available should be marked so they are not requested further, an a 'not available' image should be showed instead . 

Default source should be OSM

pre downloading tiles for all zoom levels for a specific geocoordinate should be evaluated, based on the widgets performance. Maybe different policies could be implemented aused depending on some variable/connection speed.

*/
struct longPoint
{
	quint32 x;
	quint32 y;
	longPoint(quint32,quint32);
};


class myMercator
{
	public:
	static longPoint geoCoordToPixel(QPointF const &,int , int);
	static QPointF pixelToGeoCoord(longPoint const &, int, int);
};

struct tileSet
{
	int zoom;
	quint32 top;
	quint32 bottom;
	quint32 left;
	quint32 right;
	int offsetx;
	int offsety;
};

class cacaMap : public QWidget
{

Q_OBJECT
public:	
	cacaMap(QWidget * _parent=0);
	~cacaMap();
	bool setGeoCoords(QSizeF);
private:
	QPoint mouse;
	int zoom;
	int tileSize;
	//check QtMobility QGeoCoordinate
	//latitude, longitude
	QPointF geocoords;
	QImage image;
	QNetworkAccessManager *manager;
	tileSet tilesToRender;

	void updateTilesToRender();
protected:
	
	void resizeEvent(QResizeEvent*);
	void paintEvent(QPaintEvent*);
	void onMouseDrag(QMouseEvent*);
	void onMouseRelease(QMouseEvent*);
	void downloadPicture(QString const &);

public slots:
	void dlProgress(qint64, qint64);
	void downloadReady(QNetworkReply *);
	void slotError(QNetworkReply::NetworkError);
};
#endif
