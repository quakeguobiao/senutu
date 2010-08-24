#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtCore/QStringList>
#include "CAudioCtrl.h"
#include "PlayList.h"
#include "ui_MainWindow.h"

class ControlPanel;

class Senutu : public QMainWindow,public Ui::MainWindow
{
	Q_OBJECT
public:
	Senutu(QWidget *parent = 0, Qt::WFlags flags = 0);
	QStringList getMusicList();
	int getCurrentIndex();
	void setCurrentIndex(int index);

private slots:
	void openMusicFile();

private:
	ControlPanel *m_pControlPanel;
	PlayList *m_pPlayList;
	QStringList m_MusicList;
	int m_currentIndex;

	void createConnections();

};

#endif // MAINWINDOW_H
