#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_senutu.h"
#include "ControlPanel.h"
#include "Menu.h"
#include "PlayList.h"

class Senutu : public QMainWindow
{
	Q_OBJECT

public:
	Senutu(QWidget *parent = 0, Qt::WFlags flags = 0);
	//~Senutu();

private:
	void SetupUi();
	Ui::SenutuClass ui;
	ControlPanel *m_pControlPanel;
	Menu *m_pMenu;
	PlayList *m_pPlayList;
	QMenuBar *MenuBar;


};

#endif // MAINWINDOW_H
