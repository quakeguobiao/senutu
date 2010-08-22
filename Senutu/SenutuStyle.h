#pragma once
#ifndef _SENUTUSTYLE_H
#define _SENUTUSTYLE_H

#include <QtGui/QWindowsStyle>

class SenutuStyle : public QWindowsStyle
{
public:
	SenutuStyle();
	void drawHoverRect(QPainter *painter, const QRect &rect) const;
	void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget)const;
	QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt,SubControl sc, const QWidget *widget) const;
	QSize sizeFromContents(ContentsType type, const QStyleOption *option,const QSize &size, const QWidget *widget) const;
};

#endif