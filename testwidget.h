/*
Copyright 2010 Jean Fairlie jmfairlie@gmail.com

This program  is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#ifndef TESTWIDGET_H
#define TESTWIDGET_H

#include <QtGui>
#include "myderivedmap.h"
#include <iostream>

using namespace std;

class testWidget: public QWidget
{
Q_OBJECT
public:
	testWidget(QWidget* parent=0);
	~testWidget();
private:
	myDerivedMap*  map;
	QHBoxLayout * layout;
	QSlider * slider;
protected:
	void paintEvent(QPaintEvent*);
private slots:
	void setZoom(int);
};
#endif
