#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QtGui/QWidget>
#include <QtGui/QTableWidget>
#include "CAudioCtrl.h"

#include "ui_PlayList.h"

class PlayList:public QWidget,public Ui::PlayList
{
	Q_OBJECT
public:
	PlayList(QWidget* parent = 0);
	void AddMusic(TAG &musicInfo);
};


#endif // PLAYLIST_H
