#include "SenutuStyle.h"
#include <QtGui>
#include <QtCore>
#include <QtGui/QWidget>
#include <QtGui/QPixmapCache>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QStyleOption>

QPixmap cached(const QString &img)
{
	if (QPixmap *p = QPixmapCache::find(img))
		return *p;

	QPixmap pm;
	pm = QPixmap::fromImage(QImage(img), Qt::OrderedDither | Qt::OrderedAlphaDither);
	if (pm.isNull())
		return QPixmap();

	QPixmapCache::insert(img, pm);
	return pm;
}

SenutuStyle::SenutuStyle(): QWindowsStyle()
{
	Q_INIT_RESOURCE(senutu);
}

void SenutuStyle::drawHoverRect(QPainter *painter, const QRect &r) const
{
	qreal h = r.height();
	qreal h2 = r.height() / qreal(2);
	QPainterPath path;
	path.addRect(r.x() + h2, r.y() + 0, r.width() - h2 * 2, r.height());
	path.addEllipse(r.x(), r.y(), h, h);
	path.addEllipse(r.x() + r.width() - h, r.y(), h, h);
	path.setFillRule(Qt::WindingFill);
	painter->setPen(Qt::NoPen);
	painter->setBrush(QColor(191, 215, 191));
	painter->setRenderHint(QPainter::Antialiasing);
	painter->drawPath(path);
}

void SenutuStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
	switch(control){
		case CC_Slider:
			if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
				QRect groove = subControlRect(CC_Slider, option, SC_SliderGroove, widget);
				QRect handle = subControlRect(CC_Slider, option, SC_SliderHandle, widget);
				painter->save();

				if ((option->subControls & SC_SliderGroove) && groove.isValid()) {
					QPixmap grv = cached(":res/images/slider_bar.png");
					painter->drawPixmap(QRect(groove.x() + 5, groove.y(),groove.width() - 10, grv.height()),grv);
				}
				if ((option->subControls & SC_SliderHandle) && handle.isValid()) {
					QPixmap hndl = cached(":res/images/slider_thumb_on.png");
					painter->drawPixmap(handle.topLeft(), hndl);
				}

				painter->restore();
			}
			break;
		default:
			QWindowsStyle::drawComplexControl(control, option, painter, widget);
			break;
	}
	return;
}

QRect SenutuStyle::subControlRect(ComplexControl control, const QStyleOptionComplex *option, SubControl subControl, const QWidget *widget) const
{
	QRect rect;
	rect = QWindowsStyle::subControlRect(control, option, subControl, widget);
	if (control == CC_Slider && subControl == SC_SliderHandle) {
		rect.setWidth(12);
		rect.setHeight(14);
	} else if (control == CC_Slider && subControl == SC_SliderGroove) {
		rect.setHeight(9);
		rect.moveTop(14/2 - 9/2);
	}
	return rect;
}

QSize SenutuStyle::sizeFromContents(ContentsType type, const QStyleOption *option,const QSize &size, const QWidget *widget)const
{
	QSize newSize = QWindowsStyle::sizeFromContents(type, option, size, widget);
	newSize.setHeight(14);
	return newSize;
}