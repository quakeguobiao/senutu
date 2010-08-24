#include <QtGui>
#include "ControlPanel.h"
#include "ui_ControlPanel.h"
#include "SenutuStyle.h"
#include "CAudioCtrl.h"
#include "Lyrics.h" 
#include "MainWindow.h"
#include "aerror.h"

ControlPanel::ControlPanel( Senutu * parent ):QWidget(parent)
{
	setupUi(this);
	m_pSenutu = parent;
	m_bPlayState = ControlPanel::Stopped;
	m_bPlayIcon = style()->standardIcon(QStyle::SP_MediaPlay);
	m_bPauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
	m_pPlayButton->setIcon(m_bPlayIcon);

	m_pStopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
	m_pStopButton->setEnabled(false);

	m_pRewindButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));

	m_pForwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));

	m_bVolumeIcon = style()->standardIcon(QStyle::SP_MediaVolume);
	m_bMuteIcon = style()->standardIcon(QStyle::SP_MediaVolumeMuted);
	m_pVolumeState->setIcon(m_bVolumeIcon);
	m_pVolumeState->setFlat(true);

	SenutuStyle *style = new SenutuStyle;
	m_pVolumeSlider->setStyle(style);
	delete style;
	m_pPlayInfo->setStyleSheet("border-image:url(:/res/images/screen.png) ; border-width:3px");
	m_pPlayInfo->setText(tr("<center>No media</center>"));

	ControlPanel::createConnections();
}

void ControlPanel::playORpause()
{
	if( m_bPlayState == ControlPanel::Playing)
	{
		m_bPlayState = ControlPanel::Paused;
		m_pPlayButton->setIcon(m_bPlayIcon);
		m_pPlayButton->setEnabled(true);
		m_pStopButton->setEnabled(true);
		CAudioCtrl::Pause();
	}
	else if( m_bPlayState == ControlPanel::Paused )
	{
		m_bPlayState = ControlPanel::Playing;
		m_pPlayButton->setIcon(m_bPauseIcon);
		m_pPlayButton->setEnabled(true);
		m_pStopButton->setEnabled(true);
		CAudioCtrl::Play();
	}
	else if ( m_bPlayState == ControlPanel::Stopped )
	{
		int currentIndex = m_pSenutu->getCurrentIndex();
		if( currentIndex > 0 )
		{
			int currentIndex = m_pSenutu->getCurrentIndex();
			m_bPlayState = ControlPanel::Playing;
			m_pPlayButton->setIcon(m_bPauseIcon);
			m_pPlayButton->setEnabled(true);
			m_pStopButton->setEnabled(true);
			//ControlPanel::play(currentIndex);
			QStringList QFileNames = m_pSenutu->getMusicList();
			QString QFileName = QFileNames[currentIndex-1];
			std::string sFN = QFileName.toStdString();
			char* strFileName = const_cast<char*>(sFN.c_str());
			int length = MultiByteToWideChar(CP_ACP, 0, strFileName, -1, NULL, 0); 
			wchar_t * fileName = new wchar_t[length+1]; 
			MultiByteToWideChar(CP_ACP, 0, strFileName, -1, fileName, length);  
			CAudioCtrl::Open(fileName);
			CAudioCtrl::Play();
			SAFE_DELETE_ARRAY(fileName);
		}
		else
		{
			//TBD
		}

	}

}

void ControlPanel::stop()
{
	if( m_bPlayState == ControlPanel::Playing || m_bPlayState == ControlPanel::Paused )
	{
		m_bPlayState = ControlPanel::Stopped;
		m_pPlayButton->setIcon(m_bPlayIcon);
		m_pPlayButton->setEnabled(true);
		m_pStopButton->setEnabled(false);
		CAudioCtrl::Stop();
	}
}

void ControlPanel::play(int index)
{
	QStringList QFileNames = m_pSenutu->getMusicList();
	QString QFileName = QFileNames[index-1];
	std::string sFN = QFileName.toStdString();
	char* strFileName = const_cast<char*>(sFN.c_str());
	int length = MultiByteToWideChar(CP_ACP, 0, strFileName, -1, NULL, 0); 
	wchar_t * fileName = new wchar_t[length+1]; 
	MultiByteToWideChar(CP_ACP, 0, strFileName, -1, fileName, length);   
	CAudioCtrl::Open(fileName);
	CAudioCtrl::Play();
	SAFE_DELETE_ARRAY(fileName);
}

void ControlPanel::createConnections()
{
	connect(m_pPlayButton,SIGNAL(clicked()),this,SLOT(playORpause()));
	connect(m_pStopButton,SIGNAL(clicked()),this,SLOT(stop()));

}