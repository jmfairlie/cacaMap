/*
Copyright 2010 Jean Fairlie jmfairlie@gmail.com

CacaMap is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/


/** @file cacamap.h
* CacaMap is a Simple Qt OSM Map Widget 
*/

#ifndef CACAMAP_H
#define CACAMAP_H

#include <QtGui>
#include <QtNetwork>
#include <iostream>
#include <vector>


/*
*tile server id
*/
enum SERVER_TYPE {SERVER_OSM, SERVER_TAH,SERVER_MFF, SERVER_KARTAT02,SERVER_GMAPS, SERVER_GLAYER, SERVER_GSAT};

/**
* The quint32 version of QPoint
*/

struct longPoint
{
	quint32 x;/**< x coord. */
	quint32 y;/**< y coord.*/
	longPoint(quint32,quint32);
};

/**
Helper struct that handles coordinate transformations
*/
struct myMercator
{
	static longPoint geoCoordToPixel(QPointF const &,int , int);
	static QPointF pixelToGeoCoord(longPoint const &, int, int);
};
/**
* Struct to define a range of consecutive tiles
* It's used to identify which tiles are visible and need to be rendered/downlaoded
* @see cacaMap::updateTilesToRender()
*/
struct tileSet
{
	int zoom;/**< zoom level.*/
	qint32 top;/**< topmost row.*/
	qint32 bottom;/**< bottommost row.*/
	qint32 left;/**< leftmostcolumn. */
	qint32 right;/**< rightmost column. */
	int offsetx;/**< horizontal offset needed to align the tiles in the wiget.*/
	int offsety;/**< vertical offset needed to align the tiles in the widget.*/
};

/**
* Used to represent a specific %tile
* @see cacaMap::tileCache
*/
struct tile
{
	int zoom;/**< zoom level.*/
	qint32 x;/**< colum number.*/
	qint32 y;/**< row number.*/
	QString  url;/**<used to identify the %tile when it finishes downloading.*/
};
/**
* maximum space allowed for caching tiles
*/
#define CACHE_MAX 1*1024*1024 //1MB
/**
Main map widget
*/

/** @note

INITIAL REQUIREMENTS:

Class should retrieve the tiles from the specified source (e.g. OSM, google)
and cache them in the hard drive.
Default source should be OSM.
Class should keep track of which tiles are already on the cache in order to minimize downloads.

The cache should have a maximum size (e.g 1, 2MB), when reached, tiles not in use should be deleted

The current location should be depicted always in the middle of the widget.
If Widget is bigger than the tiles to be shown or one of the edges is visible 
then tiles should be repeated horizontally and/or vertically as needed

Map should be draggable, when dragging or zooming images that are not cached should be queued 
for download, and a 'loading' place holder image should be shown in their place
Images that are not available should be marked so they are not requested further, an 
a 'not available' image should be showed instead . 


pre downloading tiles for all zoom levels for a specific geocoordinate should be evaluated, 
based on the widgets performance. Maybe different policies could be implemented to handle 
different situations e.g. connection speeds.
*/

class cacaMap : public QWidget
{

Q_OBJECT

public:	
	cacaMap(QWidget * _parent=0);

	virtual ~cacaMap();
	void setGeoCoords(QPointF);
	void renderMap(QPainter &);
	bool zoomIn();
	bool zoomOut();
	bool setZoom(int level);
	QPointF getGeoCoords();


private:
	QPoint mouseAnchor;/**< used to keep track of the last mouse click location.*/
	QNetworkAccessManager *manager;/**< manages http requests. */
	tileSet tilesToRender;/**< range of visible tiles. */
	QHash<QString,int> tileCache;/**< list of cached tiles (in HDD). */
	QHash<QString,tile> downloadQueue;/**< list of tiles waiting to be downloaded. */
	QHash<QString,int> unavailableTiles;/**< list of tiles that were not found on the server.*/
	bool downloading;/**< flag that indicates if there is a download going on. */
	QString folder;/**< root application folder. */
	QMovie loadingAnim;/**< to show a 'loading' animation for yet unavailable tiles. */
	QImage notAvailableTile;
        int minZoom;/**< Minimum zoom level (farthest away).*/
	int maxZoom;/**< Maximum zoom level (closest).*/
	SERVER_TYPE server;	
	void updateTilesToRender();
	void loadCache();
	QString getTileUrl(int, int, int);
	QString getTilePath(int, qint32);
	QString tileCacheFolder();
	QString fileExtension();


protected:
	int zoom;/**< Map zoom level. */
	int tileSize; /**< size in px of the square %tile. */
	quint32 cacheSize;/**< current %tile cache size in bytes. */
	//check QtMobility QGeoCoordinate
	QPointF geocoords; /**< current longitude and latitude. */

	void resizeEvent(QResizeEvent*);
	void paintEvent(QPaintEvent *);
	void mousePressEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void downloadPicture();

protected slots:
	void slotDownloadProgress(qint64, qint64);
	void slotDownloadReady(QNetworkReply *);
	void slotError(QNetworkReply::NetworkError);
};
#endif
