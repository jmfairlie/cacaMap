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
#include "testwidget.h"

testWidget::testWidget(QWidget* parent):QWidget(parent)
{
	map = new myDerivedMap(this);
	slider = new QSlider(Qt::Vertical,this);
	slider->setTickPosition(QSlider::TicksBothSides);
	slider->setMaximum(18);
	slider->setMinimum(0);
	connect(slider, SIGNAL(valueChanged(int)),this, SLOT(setZoom(int)));



	layout = new QHBoxLayout(this);
	layout->addWidget(slider);
	layout->addWidget(map);
	setLayout(layout);
	QSize size(384,384);
	resize(size);
}

void testWidget::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	//painter.setViewport(map->rect());
	//map->renderMap(painter);
	
	//painter.drawRect(10,10,110,110);
	//cout<<"paint testwidget!"<<endl;
}

testWidget::~testWidget()
{
	delete map;
	delete layout;
	delete slider;
}

void testWidget::setZoom(int level)
{
	map->setZoom(level);
	map->update();
}
