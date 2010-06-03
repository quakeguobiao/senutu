#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QTableWidget>

class PlayList:public QWidget{
	Q_OBJECT

public:
	PlayList(QWidget* parent = 0);
	void AddPlayList();
	//~PlayList();
	QTableWidget* GetWidget();
private:
	QTableWidget* m_pMusicTable;

};


#endif // PLAYLIST_H
