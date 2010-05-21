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


#ifndef _SRVRMGR
#define _SRVRMGR

#include <QtXml>
struct tileserver
{
	QString name;/**<name of the tile server*/
	QString url;/**< url template for accessing tiles*/
	QString folder;/**< name of folder where tiles will be stored*/
	QString path;/**< path where tiles will be stored*/
	QString tile;/**< tile file*/ 
};

class servermanager
{
public:
	bool loadConfigFile(QString);
	QString getTileUrl(int,quint32,quint32);
	QString tileCacheFolder();
	//returns the filename of the file as it should be stored in HD
	QString fileName(quint32);
	void selectServer(int);
	QString serverName();
	QString filePath(int, quint32);
	QStringList getServerNames();

private:
	QVector<tileserver> serverlist;/**< list of server structs*/
	int selectedServer;/**< index in list of current server*/
	QStringList serverNames;/**< names of servers in xml file*/
};

#endif
