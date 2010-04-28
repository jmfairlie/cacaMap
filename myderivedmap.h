#ifndef DERIVEDMAP_H
#define DERIVEDMAP_H


#include "cacamap.h"

class myDerivedMap: public cacaMap
{
	public:
	myDerivedMap(QWidget*);
	protected:
	void paintEvent(QPaintEvent *);
};
#endif
