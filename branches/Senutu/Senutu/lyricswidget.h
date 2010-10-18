#ifndef LYRICSWIDGET_H
#define LYRICSWIDGET_H

#include <QWidget>
#include "ui_lyricswidget.h"

class LyricsWidget : public QWidget
{
	Q_OBJECT

public:
	LyricsWidget(QWidget *parent = 0);
	~LyricsWidget();

private:
	Ui::LyricsWidgetClass ui;
};

#endif // LYRICSWIDGET_H
