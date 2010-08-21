#include <QtGui>
#include "Menu.h"

#include "CAudioCtrl.h"

Menu::Menu(QWidget* parent)
{
	AddFileAction = new QAction(tr("Add &File"), parent);
    AddFileAction->setShortcut(tr("Ctrl+O"));
	AddFolderAction = new QAction(tr("Add Folder"),parent);
	ExitAction = new QAction(tr("&Exit"), parent);
	ExitAction->setShortcuts(QKeySequence::Quit);
}

void Menu::SetupMenuBar(QMenuBar* MenuBar)
{
	FileMenu = MenuBar->addMenu(tr("&File"));
	FileMenu->addAction(AddFileAction);
	FileMenu->addAction(AddFolderAction);
    FileMenu->addSeparator();
    FileMenu->addAction(ExitAction);

	connect(AddFileAction, SIGNAL(triggered()), this, SLOT(AddFile()));
 
}

void Menu::AddFile()
{
// 	QStringList FileName = QFileDialog::getOpenFileNames(this, tr("Select Music Files"),
// 				QDesktopServices::storageLocation(QDesktopServices::MusicLocation));
}