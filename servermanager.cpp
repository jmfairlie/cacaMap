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
#include "servermanager.h"
#include <iostream>
using namespace std;

/**
* loads info from xml file
* @return true if succesful false otherwise
*/
bool servermanager::loadConfigFile(QString xmlfile)
{
  QDomDocument doc("mydocument");
  QFile file(xmlfile);

  if (!file.open(QIODevice::ReadOnly))
  {
        std::cout<<"couldn't open file"<<std::endl;
	return false;
  }

  if (!doc.setContent(&file)) 
  {
  	file.close();
	cout<<"couldn't set content"<<endl;
	return false;
  }

  QDomNodeList servers = doc.elementsByTagName("server");
  if(!servers.length())
  {
  	cout<<"no servers defined in xml file"<<endl;
	return false;
  }
  for (quint32 i=0; i< servers.length(); i++)
  {
  	QDomNode server = servers.item(i);
	QDomNode namenode = server.namedItem("name");
	if (namenode.isNull())
	{
		cout<<"server has no name tag in xml file"<<endl;
		return 0;
	}

	QDomCharacterData nametext = namenode.firstChild().toCharacterData();
	if (nametext.isNull())
	{
		cout<<"server name is empty in xml"<<endl;
		return 0;
	}
	serverNames.append(nametext.data());
	
	QDomNode urlnode = server.namedItem("url");
	QDomCDATASection urltext = urlnode.firstChild().toCDATASection();
	if (urltext.isNull())
	{
		cout<<"url format is empty in xml"<<endl;
		return 0;
	}
	
	QDomNode foldernode = server.namedItem("folder");
	QDomCharacterData foldertext = foldernode.firstChild().toCharacterData();
	if (foldertext.isNull())
	{
		cout<<"folder format is empty in xml"<<endl;
		return 0;
	}

	QDomNode filepathnode = server.namedItem("filepath");
	QDomCDATASection filepathtext = filepathnode.firstChild().toCDATASection();
	if (filepathtext.isNull())
	{
		cout<<"filepath is empty in xml"<<endl;
		return 0;
	}
	
	QDomNode tilenode = server.namedItem("tile");
	QDomCDATASection tiletext = tilenode.firstChild().toCDATASection();
	if (tiletext.isNull())
	{
		cout<<"tile format is empty in xml"<<endl;
		return 0;
	}

	tileserver serveritem;
	serveritem.name = nametext.data();
	serveritem.url = urltext.data();
	serveritem.folder = foldertext.data();
	serveritem.path = filepathtext.data();
	serveritem.tile = tiletext.data();

	serverlist.append(serveritem);
  }
  selectedServer = 0;
  file.close();
  return true;
}
/**
* Get URL of a specific %tile
* @param zoom zoom level
* @return string containing the url where the %tile image can be found.
*/
QString servermanager::getTileUrl(int zoom, quint32 x, quint32 y)
{
	QString sz,sx,sy;
	sz.setNum(zoom);
	sx.setNum(x);
	sy.setNum(y);
	QString urltmpl = serverlist.at(selectedServer).url;

	urltmpl.replace(QString("%z"),sz);
	urltmpl.replace(QString("%x"),sx);
	urltmpl.replace(QString("%y"),sy);
	return urltmpl;
}

/**
* @return name of the cache folder for the given tile server
*/
QString servermanager::tileCacheFolder()
{
	return  serverlist.at(selectedServer).folder;
}

/**
* @return tile file name
*/
QString servermanager::fileName(quint32 y)
{
	QString filetmpl = serverlist.at(selectedServer).tile;
	QString sy;
	sy.setNum(y);
	filetmpl.replace("%y",sy);
	return filetmpl;
}

/**
* @return tile file path
*/
QString servermanager::filePath(int zoom, quint32 x)
{
	QString filetmpl = serverlist.at(selectedServer).path;
	QString sz, sx;
	sz.setNum(zoom);
	sx.setNum(x);
	filetmpl.replace("%z",sz);
	filetmpl.replace("%x",sx);
	return filetmpl;
}


/**
* @return server name
*/
QString servermanager::serverName()
{
	return serverlist.at(selectedServer).name;
}
/**
* selects server at index
*/

void servermanager::selectServer(int index)
{
	if (index >=0 && index < serverlist.size())
	{
		selectedServer = index;
	}
}

/**
* return list of server names
*/

QStringList servermanager::getServerNames()
{	
	return serverNames;
}
