#ifndef MENU_H
#define MENU_H

#include <QAction>
#include <QMainWindow>

class Menu:public QMainWindow
{
	Q_OBJECT
public:
	Menu(QWidget* parent = 0);
	void SetupMenuBar(QMenuBar* MenuBar);
private slots:
	void AddFile();
//	void AddFolder();
//	void Exit();
private:
	QAction *AddFileAction;
	QAction *AddFolderAction;
	QAction *ExitAction;
	QMenu *FileMenu;

};

#endif // MENU_H