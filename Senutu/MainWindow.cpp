#include <QtGui>
#include "MainWindow.h"
#include "CAudioCtrl.h"

Senutu::Senutu(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	MenuBar = menuBar();
	m_pControlPanel = new ControlPanel(this);
	m_pMenu = new Menu(this);
	m_pPlayList = new PlayList(this);
	m_pControlPanel->SetupControlPanel();
	m_pMenu->SetupMenuBar(MenuBar);
	SetupUi();

	CAudioCtrl::Init();

}

void Senutu::SetupUi()
{
	QGridLayout *MainLayout = new QGridLayout;
	MainLayout->addWidget(m_pControlPanel->GetWidget(),0,0,1,2);
	MainLayout->addWidget(m_pPlayList->GetWidget(),1,0);
	QWidget *widget = new QWidget;
	widget->setLayout(MainLayout);
	setCentralWidget(widget);
	
}

