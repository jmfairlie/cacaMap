/*
Copyright 2010 Jean Fairlie jmfairlie@gmail.com

This file is part of CacaMap
CacaMap is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "cacamap.h"

using namespace std;
/**
* constructor
*/
longPoint::longPoint(quint32 _x, quint32 _y)
{
	x = _x;
	y = _y;
}
/**
* Converts a geo coordinate to map pixels
* @param geocoord has the longitude and latitude in degrees.
* @param zoom is the zoom level, ranges from cacaMap::minZoom (out) to maxZoom (in).
* @param tilesize the width/height in px of the square %tile (e.g 256).
* @return a longpoint struct containing the x and y px coordinates
* in the map for the given geocoordinates and zoom level.
*/
longPoint myMercator::geoCoordToPixel(QPointF const &geocoord, int zoom, int tilesize)
{
 	qreal  longitude = geocoord.x();
	qreal  latitude = geocoord.y();
	
	//height, width of the whole map,this is, all tiles for a given zoom level put together
	quint32 mapsize =  (1<<zoom)*tilesize;
	
	qreal latitude_m = atanh(sin(latitude/180.0*M_PI))*180.0/M_PI;

	quint32 x = mapsize*(longitude + 180.0)/360.0;
	
	quint32 y = mapsize*(180.0 - latitude_m)/360.0;
	
	return longPoint(x,y);
}
/**
* Converts  map pixels to geo coordinates in degrees
* @param pixelcoord has the x and y px coordinates.
* @param zoom  is the zoom level, ranges from 0(out) to 18(in).
* @param tilesize the width/height in px of the square %tile (e.g 256).
* @return a QPointF object containing the latitude and longitude of 
* of the given location.
*/

QPointF myMercator::pixelToGeoCoord(longPoint const &pixelcoord, int zoom, int tilesize)
{
	long  x= pixelcoord.x;
	long  y= pixelcoord.y;
	
	//height, width of the whole map,this is, all tiles for a given zoom level put together
	quint32 mapsize =  (1<<zoom)*tilesize;

	
	qreal longitude = x*360.0/mapsize - 180.0;
	
	qreal latitude_m = 180.0 - y*360.0/mapsize;
	qreal latitude = asin(tanh(latitude_m*M_PI/180.0))*180/M_PI;
	
	return QPointF(longitude,latitude);
}
/**
* constructor
*/
cacaMap::cacaMap(QWidget* parent):QWidget(parent)
{
	server = SERVER_GMAPS;
	cacheSize = 0;
	maxZoom = 18;
	minZoom = 0;
	folder = QDir::currentPath();
	loadCache();
	geocoords = QPointF(23.5,61.5);
	downloading = false;
	tileSize = 256;
	zoom = 4;
	manager = new QNetworkAccessManager(this);
	loadingAnim.setFileName("loading.gif");
	loadingAnim.setScaledSize(QSize(tileSize,tileSize));
	loadingAnim.start();
	notAvailableTile.load("notavailable.jpeg");
}

/**
Sets the latitude and longitude to the coords in newcoords
@param newcoords the new coordinates.
*/
void cacaMap::setGeoCoords(QPointF newcoords)
{
	//geocoords.setX(newcoords.x());
	//geocoords.setY(newcoords.y());
	geocoords = newcoords;
}

/**
* zooms in one level
* @return true if it zoomed succesfully, false otherwise (if maxZoom has been reached)
*/
bool cacaMap::zoomIn()
{
	if (zoom < maxZoom)
	{
		zoom++;
		updateTilesToRender();
		return true;
	}
	return false;
}
/**
* zooms out one level
* @return true if it zoomed succesfully, false otherwise (if minZoom has been reached)
*/
bool cacaMap::zoomOut()
{
	if (zoom > minZoom)
	{
		zoom--;
		updateTilesToRender();
		return true;
	}
	return false;
}

/**
* zooms to a specific level
* @return true if it is a valid level, false otherwise (if level is outside the valid range)
*/
bool cacaMap::setZoom(int level)
{
	if (level>= minZoom && level <= maxZoom)
	{
		zoom = level;
		updateTilesToRender();
		return true;
	}
	return false;
}

/**
* @return current geocoords
*/
QPointF cacaMap::getGeoCoords()
{
	return geocoords;
}

/**
Saves the screen coordinates of the last click
This is used for scrolling the map
@see cacaMap::mouseMoveEvent()
*/
void cacaMap::mousePressEvent(QMouseEvent* e)
{
	mouseAnchor = e->pos();
}

/**
Calculates the length of the mouse drag and
translates it into a new coordinate, map is rerendered
*/
void cacaMap::mouseMoveEvent(QMouseEvent* e)
{
	QPoint delta = e->pos()- mouseAnchor;
	mouseAnchor = e->pos();
	longPoint p = myMercator::geoCoordToPixel(geocoords,zoom,tileSize);
	
	p.x-= delta.x();
	p.y-= delta.y();
	geocoords = myMercator::pixelToGeoCoord(p,zoom,tileSize);
	updateTilesToRender();
	update();
}


/**
* Get URL of a specific %tile
* @param zoom zoom level
* @return string containing the url where the %tile image can be found.
*/
QString cacaMap::getTileUrl(int zoom, int x, int y)
{
	QString sz,sx,sy;
	sz.setNum(zoom);
	sx.setNum(x);
	sy.setNum(y);
	QString surl;
	switch(server)
	{
		case SERVER_OSM:
			surl= "http://tile.openstreetmap.org/"+sz+"/"+sx+"/"+sy+".png";
			break;
		case SERVER_TAH:
			surl= "http://tah.openstreetmap.org/Tiles/tile/"+sz+"/"+sx+"/"+sy+".png";
			break;
		case SERVER_MFF:
			surl= "http://maps-for-free.com/layer/relief/z"+sz+"/row"+sy+"/"+sz+"_"+sx+"-"+sy+".jpg";
			break;
		case SERVER_KARTAT02:
			surl= "http://ed-map-fi.wide.basefarm.net/tiles//aerial/en_FI/"+sz+"/"+sx+"/"+sy+".jpeg";
			break;
		case SERVER_GMAPS:
			surl="http://mt1.google.com/vt/x="+sx+"&y="+sy+"&z="+sz;
			break;
		case SERVER_GLAYER:
			surl="http://mt1.google.com/vt/lyrs=h@126&x="+sx+"&y="+sy+"&z="+sz;
			break;
		case SERVER_GSAT:
			surl="http://khm3.google.com/kh/v=60&x="+sx+"&y="+sy+"&z="+sz;
			break;
		default:
			surl= QString("http://tile.openstreetmap.org/")+sz+"/"+sx+"/"+sy+".png";
			break;
	}
	return surl;
}

/**
*@param zoom zoom level
*@param x tile x column
*@return string with the path to the folder containing the tiles in 
*for zoom level and x column
*/
QString cacaMap::getTilePath(int zoom,qint32 x)
{
	return "cache/"+tileCacheFolder()+"/"+QString().setNum(zoom) +"/"+QString().setNum(x)+"/";
}
/**
*@return name of the cache folder for the given tile server
*/
QString cacaMap::tileCacheFolder()
{
	switch(server)
	{
		case SERVER_OSM:
			return "osm";
			break;
		case SERVER_TAH:
			return "tah";
			break;
		case SERVER_MFF:
			return "mff";
			break;
		case SERVER_KARTAT02:
			return "02";
			break;
		case SERVER_GMAPS:
			return "gmaps";
			break;
		case SERVER_GLAYER:
			return "glayer";
			break;
		case SERVER_GSAT:
			return "gsat";
			break;
		default:
			return "osm";
		break;
	}
}

/**
* @return image format (e.g. png,jpg).
*/
QString cacaMap::fileExtension()
{
	switch(server)
	{
		case SERVER_OSM:
			return "png";
			break;
		case SERVER_TAH:
			return "png";
			break;
		case SERVER_MFF:
			return "jpg";
			break;
		case SERVER_KARTAT02:
			return "jpeg";
			break;
		case SERVER_GMAPS:
			return "";
			break;
		case SERVER_GLAYER:
			return "";
			break;
		case SERVER_GSAT:
			return "";
			break;


		default:
			return "png";
		break;
	}
}



/**
Starts downloading the next %tile in the queue
@see cacaMap::downloadQueue
*/
void cacaMap::downloadPicture()
{
	//check if there isnt an active download already
	if (!downloading)
	{
		//there are items in the queue
		if (downloadQueue.size())
		{
			downloading = true;
			QHash<QString,tile>::const_iterator i;
			i = downloadQueue.constBegin();
			tile nextItem = i.value();
			QString surl = nextItem.url;
			QNetworkRequest request;
			request.setUrl(QUrl(surl));
			QNetworkReply *reply = manager->get(request);
			connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),this, SLOT(slotError(QNetworkReply::NetworkError)));
			connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(slotDownloadReady(QNetworkReply*)));
			connect(reply, SIGNAL(downloadProgress(qint64,qint64)),this, SLOT(slotDownloadProgress(qint64, qint64)));
		}
		else
		{
			cout<<"no items in the queue"<<endl;
		}
	}
	else
	{
		cout<<"another download is already in progress... "<<endl;
	}
}
/**
Populates the cache list by checking the existing files on the cache folder
*/
void cacaMap::loadCache()
{
	QDir::setCurrent(folder);
	QDir dir;
	if (dir.cd("cache"))
	{
		if (dir.cd(tileCacheFolder()))
		{
			QStringList zoom = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
			QString zoomLevel;
			for(int i=0; i< zoom.size(); i++)
			{
				zoomLevel = zoom.at(i);
				dir.cd(zoomLevel);
				QStringList longitudes = dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
				QString lon;
				for(int j=0; j< longitudes.size(); j++)
				{
					lon = longitudes.at(j);
					dir.cd(lon);
					QFileInfoList latitudes = dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);
					QString lat;
					for(int k=0; k< latitudes.size(); k++)
					{
						lat = latitudes.at(k).baseName();
						cacheSize+= latitudes.at(k).size();
						QString name = zoomLevel+"."+lon+"."+lat;
						tileCache.insert(name,1);
					}
					dir.cdUp();//go back to zoom level folder
				}
				dir.cdUp();//go back to tile folder
			}
		}
		QDir::setCurrent(folder);
		cout<<"cache size "<<(float)cacheSize/1024/1024<<" MB"<<endl;
	}
}

/**
Slot to keep track of download progress
*/
void cacaMap::slotDownloadProgress(qint64 _bytesReceived, qint64 _bytesTotal)
{
	update();
}

/**
Slot that gets called everytime a %tile download request finishes
Saves image file to HDD, takes out item from download queue, and  adds item to cache list
*/
void cacaMap::slotDownloadReady(QNetworkReply * _reply)
{
	QNetworkReply::NetworkError error = _reply->error();
	//get url of original request 
	QNetworkRequest req = _reply->request();
	QUrl url = req.url();
	QString surl = url.toString();
	bool found = false;
	QHash<QString,tile>::const_iterator i;
	i = downloadQueue.constBegin();
	
	for( i; i!=downloadQueue.constEnd();i++)
	{
		if (i.value().url == surl)
		{
			found = true;
			break;
		}
	}

	if (error == QNetworkReply::NoError)
	{
		cout<<"no error"<<endl;
		qint64 bytes = _reply->bytesAvailable();

		if (bytes)
		{
			//get url of original request 
			QNetworkRequest req = _reply->request();
			QUrl url = req.url();
			QString surl = url.toString();
			cacheSize+=bytes;
			//get image data
			QByteArray data = _reply->readAll();		
			if (found)
			{
				tile nextItem = i.value();
				QString kk = i.key();
				QString zdir = QString().setNum(nextItem.zoom);
				QString xdir = QString().setNum(nextItem.x);
				QString tilefile = QString().setNum(nextItem.y)+"."+fileExtension();

				QDir::setCurrent(folder);
				QDir dir;
				if (!dir.exists("cache"))
				{
					dir.mkdir("cache");
				}
				dir.cd("cache");
				if (!dir.exists(tileCacheFolder()))
				{
					dir.mkdir(tileCacheFolder());
				}
				dir.cd(tileCacheFolder());

				if(!dir.exists(zdir))
				{
					dir.mkdir(zdir);	
				}
				dir.cd(zdir);
				if(!dir.exists(xdir))
				{
					dir.mkdir(xdir);	
				}
				dir.cd(xdir);
				
				QDir::setCurrent(dir.path());
				QFile f(tilefile);
				f.open(QIODevice::WriteOnly);
				quint64 byteswritten = f.write(data);
				if (byteswritten <= 0)
				{
					cout<<"error writing to file "<<f.fileName().toStdString()<<endl;
				}
				f.close();
				//remove item from download queue
				downloadQueue.remove(i.key());
				
				//add it to cache
				tileCache.insert(kk,1);
			}
			else
			{
				cout<<"downloaded tile "<<surl.toStdString()<<" was not in Download queue. Data ignored"<<endl;
			}
			downloading = false;
			downloadPicture();
		}
		else
		{
			cout<<"no data"<<endl;
		}
		update();
	}
	else
	{
		cout<<"network error: "<<error<<endl;
		//if content is not available we dont want to keep requesting it
		
		if(found)
		{
			if (error == QNetworkReply::ContentNotFoundError)
			{
				unavailableTiles.insert(i.key(),1);
				downloadQueue.remove(i.key());
			}
			downloading = false;
			downloadPicture();
		}
	}
	_reply->deleteLater();
}
/**
Slot that gets called when theres is an network error
*/
void cacaMap::slotError(QNetworkReply::NetworkError _code)
{
	cout<<"some error "<<_code<<endl;
}
/**
Widget resize event handler
*/
void cacaMap::resizeEvent(QResizeEvent* event)
{
	updateTilesToRender();
}

/**
Renders map based on range of visible tiles
*/
void cacaMap::renderMap(QPainter &p)
{
	for (qint32 i= tilesToRender.left;i<= tilesToRender.right; i++)
	{
		for (qint32 j=tilesToRender.top ; j<= tilesToRender.bottom; j++)
		{
			QString x;
			QString y;
			//wrap around the tiles horizontally if i is outside [0,2^zoom]
			qint32 numtiles = 1<<tilesToRender.zoom;
			qint32 valx =((i<0)*numtiles + i%numtiles)%numtiles;
			
			x.setNum(valx);
			
			QImage image;
			int posx = (i-tilesToRender.left)*tileSize - tilesToRender.offsetx;
			int posy =  (j-tilesToRender.top)*tileSize - tilesToRender.offsety;
			//dont try to render tiles with y coords outside range
			//cause we cant do vertical wrapping!
			if (j>=0 && j<numtiles)
			{
				QString tileid = QString().setNum(tilesToRender.zoom) +"."+x+"."+QString().setNum(j);
				if (tileCache.contains(tileid))
				{
					//render the tile
					QDir::setCurrent(folder);
					//check path format (windows?)
					QString path= getTilePath(tilesToRender.zoom,valx) ;
					QString fileName = QString().setNum(j)+"."+fileExtension();
					QDir::setCurrent(path);
					QFile f(fileName);
					if (f.open(QIODevice::ReadOnly))
					{
						image.loadFromData(f.readAll());
						f.close();
										}
					else
					{
						cout<<"no file found "<<path.toStdString()<<endl;
					}
				}
				//check if it's in the list of unavailable tiles
				else if (unavailableTiles.contains(tileid))
				{
					image = notAvailableTile;
				}
				//the tile is not cached so download it
				else
				{
					//check that the image hasnt been queued already
					if (!downloadQueue.contains(tileid))
					{
						tile t;
						t.zoom = tilesToRender.zoom;
						t.x = valx;
						t.y = j;
						t.url = getTileUrl(tilesToRender.zoom,valx,j);
						//queue the image for download
						downloadQueue.insert(tileid,t);
					}
					image = loadingAnim.currentImage();
				}
				p.drawImage(posx,posy,image);
			}
			//p.drawRect(posx,posy,tileSize, tileSize);
		}
	}
	p.drawRect(0,0,width()-1, height()-1);
	if (!downloading)
	{
		downloadPicture();
	}
}
/**
Paint even handler
*/
void cacaMap::paintEvent(QPaintEvent *event)
{
	QPainter p(this);
	renderMap(p);
}

/**
destructor
*/
cacaMap::~cacaMap()
{
	delete manager;
}
/**
Figures out which tiles are visible
Based on the current size of the widget, the %tile size, current coordinates and zoom level
*/
void cacaMap::updateTilesToRender()
{
	longPoint pixelCoords = myMercator::geoCoordToPixel(geocoords,zoom,tileSize); 

	//central tile coords
	qint32 xtile = pixelCoords.x/tileSize;
	qint32 ytile = pixelCoords.y/tileSize;
	//offset of central tile respect to the center of the widget
	int offsetx = pixelCoords.x % tileSize;
	int offsety = pixelCoords.y % tileSize;

	//num columns of tiles that fit left of the central tile
	float tilesleft = (float)(this->width()/2 - offsetx)/tileSize;
	
	//how many pixels overflow from the leftmost  tiles
	//second %tileSize is to take into account negative tilesLeft
	int globaloffsetx = (tileSize - (this->width()/2 - offsetx) % tileSize)%tileSize;

	//num rows of tiles that fit above the central tile
	float tilesup = (float)(this->height()/2 - offsety)/tileSize;

	//how many pixels overflow from top tiles
	int globaloffsety = (tileSize - (this->height()/2 - offsety) % tileSize)%tileSize;

	//num columns of tiles that fit right of central tile
	float tilesright = (float)(this->width()/2 + offsetx - tileSize)/tileSize;
	//num rows of tiles that fit under central tile
	float tilesbottom = (float)(this->height()/2 + offsety - tileSize)/tileSize;

	tilesToRender.left = xtile - ceil(tilesleft);
	tilesToRender.right = xtile + ceil(tilesright);
	tilesToRender.top =ytile - ceil(tilesup);
	tilesToRender.bottom = ytile + ceil(tilesbottom);
	tilesToRender.offsetx = globaloffsetx;
	tilesToRender.offsety = globaloffsety;
	tilesToRender.zoom = zoom;
}
