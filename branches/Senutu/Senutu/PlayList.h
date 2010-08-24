#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QtGui/QWidget>
#include <QtGui/QTableWidget>
#include "CAudioCtrl.h"

#include "ui_PlayList.h"

class Senutu;

class PlayList:public QWidget,public Ui::PlayList
{
	Q_OBJECT
public:
	PlayList(Senutu* parent);
	~PlayList();
	void AddMusic(const TAG &musicInfo);

private slots:
	void TableDoubleClicked(int row, int column);

private:
	void createConnections();

	Senutu* m_pSenutu;
};


#endif // PLAYLIST_H
