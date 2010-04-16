#include "cacamap.h"

using namespace std;

longPoint::longPoint(quint32 _x, quint32 _y)
{
	x = _x;
	y = _y;
}

longPoint myMercator::geoCoordToPixel(QPointF const &geocoord, int zoom, int tilesize)
{
 	qreal  longitude = geocoord.x();
	qreal  latitude = geocoord.y();
	
	//height, width of the whole map,this is, all tiles for a given zoom level put together
	quint32 mapsize =  (2<<zoom)*tilesize;

	
	quint32 x = mapsize*(longitude + 180.0)/180.0;
	
	quint32 y = mapsize*(90.0 - latitude)/180.0;
	
	return longPoint(x,y);
}

QPointF myMercator::pixelToGeoCoord(longPoint const &pixelcoord, int zoom, int tilesize)
{
	long  x= pixelcoord.x;
	long  y= pixelcoord.y;
	
	//height, width of the whole map,this is, all tiles for a given zoom level put together
	quint32 mapsize =  (2<<zoom)*tilesize;

	
	qreal longitude = x*180/mapsize - 180;
	
	qreal latitude = 90 - y*180/mapsize;
	
	return QPointF(longitude,latitude);
}

cacaMap::cacaMap(QWidget* parent):QWidget(parent)
{
	cout<<"contructor"<<endl;
	geocoords = QPointF(0.0,0.0);
	tileSize = 256;
	QSize size(400,300);
	resize(size);
	zoom = 4;
	manager = new QNetworkAccessManager(this);
	downloadPicture("http://tile.openstreetmap.org/4/7/7.png");
}

void cacaMap::downloadPicture(QString const &_url )
{
	//when to delete the request?	
	QNetworkRequest request;
	request.setUrl(QUrl(_url));
	QNetworkReply *reply = manager->get(request);
	//connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),this, SLOT(slotError(QNetworkReply::NetworkError)));
	connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(downloadReady(QNetworkReply*)));
	connect(reply, SIGNAL(downloadProgress(qint64,qint64)),this, SLOT(dlProgress(qint64, qint64)));
}

void cacaMap::dlProgress(qint64 _bytesReceived, qint64 _bytesTotal)
{
	cout<<"progress: "<<_bytesReceived<<" "<<_bytesTotal<<endl;
}
void cacaMap::downloadReady(QNetworkReply * _reply)
{
	qint64 bytes = _reply->bytesAvailable();

	if (bytes)
	{
		cout<<"finished "<<_reply->request().url().toString().toStdString()<<endl;
		QByteArray data = _reply->readAll();
		image =  QImage::fromData(data);
	}
	else
	{
		cout<<"no data"<<endl;
	}
	_reply->deleteLater();
	update();


}

void cacaMap::slotError(QNetworkReply::NetworkError _code)
{
	cout<<"some error "<<_code<<endl;
}
void cacaMap::resizeEvent(QResizeEvent* event)
{
	updateTilesToRender();
}

void cacaMap::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.drawImage(QPoint(width()/2 - tilesToRender.offsetx, height()/2 - tilesToRender.offsety),image);
}

cacaMap::~cacaMap()
{
	delete manager;
	cout<<"destructor"<<endl;
}

void cacaMap::updateTilesToRender()
{
	longPoint pixelCoords = myMercator::geoCoordToPixel(geocoords,zoom,tileSize); 
	
	//central tile coords
	quint32 xtile = pixelCoords.x/tileSize;
	quint32 ytile = pixelCoords.y/tileSize;
	//offset of central tile respect to the center of the widget
	int offsetx = pixelCoords.x % tileSize;
	int offsety = pixelCoords.y % tileSize;

	//num columns of tiles that fit left of the central tile
	float tilesleft = (float)(this->width()/2 - offsetx)/tileSize;
	
	//how many pixels overflow from the leftmost  tiles
	int globaloffsetx = (this->width()/2 - offsetx) % tileSize;

	//num rows of tiles that fit above the central tile
	float tilesup = (float)(this->height()/2 - offsety)/tileSize;

	//how many pixels overflow from top tiles
	int globaloffsety = (this->height()/2 - offsety) % tileSize;

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

	cout<<tilesToRender.left<<" "<<tilesToRender.right<<" "<<tilesToRender.top<<" "<<tilesToRender.bottom<<" "<<tilesToRender.offsetx<<" "<<tilesToRender.offsety<<endl;
}
