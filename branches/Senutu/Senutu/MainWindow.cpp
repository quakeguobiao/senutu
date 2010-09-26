#include <QtGui>
#include <QtGui/QDialog>
#include <QtGui/QLayout>
#include <string>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ControlPanel.h"
#include "aerror.h"

Senutu::Senutu(QWidget *parent, Qt::WFlags flags): QMainWindow(parent, flags)
{
	setupUi(this);
	m_currentIndex = 0;
	m_pControlPanel = new ControlPanel(this);
	m_pPlayList = new PlayList(this);
	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addWidget(m_pControlPanel);
	MainLayout->addWidget(m_pPlayList);
	QWidget *widget = new QWidget;
	widget->setLayout(MainLayout);
	setCentralWidget(widget);
	m_MusicList.clear();
	createConnections();
	CAudioCtrl::Init();
}

QStringList Senutu::getMusicList()
{
	return m_MusicList;
}

int Senutu::getCount()
{
	return m_MusicList.count();
}

int Senutu::getCurrentIndex()
{
	return m_currentIndex;
}

void Senutu::setCurrentIndex(int index)
{
	m_currentIndex = index;
}

void Senutu::openMusicFile()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFiles);
	QStringList files;
	if (dialog.exec())
		files = dialog.selectedFiles();
	if (files.isEmpty())
		return;
	foreach (QString string, files) {
		std::string zizi = string.toStdString();
		char* strFileName = const_cast<char*>(zizi.c_str());
		int length = MultiByteToWideChar(CP_ACP, 0, strFileName, -1, NULL, 0); 
		wchar_t * fileName = new wchar_t[length+1]; 
		MultiByteToWideChar(CP_ACP, 0, strFileName, -1, fileName, length);   
		if (CAudioCtrl::Open(fileName) == AR_OK) {    //add it to the playlist only when it has been opened correctly
			TAG musicInfo = CAudioCtrl::GetTag();
			if (musicInfo.Title == "") { //if the tag is empty,display the file name
				musicInfo.Title = strFileName;
				size_t pos1 = musicInfo.Title.find_last_of('//');
				if (pos1 == std::string::npos)
					pos1 = musicInfo.Title.find_last_of('\\');
				size_t pos2 = musicInfo.Title.find_last_of('.');
				musicInfo.Title = musicInfo.Title.substr(pos1+1,pos2-pos1-1);
			}
			m_pPlayList->AddMusic(musicInfo);
			CAudioCtrl::Close();
			SAFE_DELETE_ARRAY(fileName);   //delete resources
		}

	}
	if(m_MusicList.count() == 0)
		m_currentIndex = 1;
	m_MusicList.append(files);
	m_pControlPanel->setPlayState(ControlPanel::Open);
}

void Senutu::createConnections()
{
	connect(m_pOpenAction, SIGNAL(triggered()), this, SLOT(openMusicFile()));
	connect(m_pPlayList,SIGNAL(mediaSourceChanged(int)),m_pControlPanel,SLOT(playMusic(int)));
	connect(m_pPlayList,SIGNAL(currentIndexChanged()),m_pControlPanel,SLOT(currentIndexChanged()));
	connect(m_pControlPanel,SIGNAL(rewindOrForward(int)),m_pPlayList,SLOT(rewindOrForward(int)));
}