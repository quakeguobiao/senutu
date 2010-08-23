#include <QtGui>
#include <QtGui/QFileDialog>
#include <QtGui/QDesktopServices>
#include <QtGui/QLayout>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ControlPanel.h"

Senutu::Senutu(QWidget *parent, Qt::WFlags flags): QMainWindow(parent, flags)
{
	setupUi(this);
	m_pControlPanel = new ControlPanel(this);
	m_pPlayList = new PlayList(this);
	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addWidget(m_pControlPanel);
	MainLayout->addWidget(m_pPlayList);
	QWidget *widget = new QWidget;
	widget->setLayout(MainLayout);
	setCentralWidget(widget);
	m_MusicList.clear();
	CAudioCtrl::Init();
	createConnections();

}

void Senutu::open()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFiles);
	QStringList files;
	if (dialog.exec())
		files = dialog.selectedFiles();
	/*QStringList files = QFileDialog::getOpenFileNames();*/
	if (files.isEmpty())
		return;
	foreach (QString string, files) {
		/*LPWSTR fileName = const_cast<LPWSTR>(string.toStdWString().c_str());*/
		std::string zizi = string.toStdString();
		char* strFileName = const_cast<char*>(zizi.c_str());
		int length = MultiByteToWideChar(CP_ACP, 0, strFileName, -1, NULL, 0); 
		wchar_t * fileName = new wchar_t[length+1]; 
		MultiByteToWideChar(CP_ACP, 0, strFileName, -1, fileName, length);   
		CAudioCtrl::Open(fileName);
		TAG musicInfo = CAudioCtrl::GetTag();
		m_pPlayList->AddMusic(musicInfo);
		CAudioCtrl::Close();
	//	SAFE_DELETE_ARRAY(strFileName);   //delete resources
		SAFE_DELETE_ARRAY(fileName);   //delete resources
	}
	m_MusicList.append(files);
}

void Senutu::createConnections()
{
	connect(m_pOpenAction,SIGNAL(triggered()),this,SLOT(open()));
}