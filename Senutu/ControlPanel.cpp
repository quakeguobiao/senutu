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
	m_bPlayState = ControlPanel::Open;
	m_bPlayIcon = style()->standardIcon(QStyle::SP_MediaPlay);
	m_bPauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
	m_pPlayButton->setIcon(m_bPlayIcon);

	m_pStopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
	m_pStopButton->setEnabled(false);

	m_pRewindButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
	m_pRewindButton->setEnabled(false);

	m_pForwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
	m_pForwardButton->setEnabled(false);

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

void ControlPanel::setPlayState(PlayState playstate)
{
	m_bPlayState = playstate;
	switch (playstate){
		case Playing:
		{
			m_bPlayState = ControlPanel::Playing;
			m_pPlayButton->setIcon(m_bPauseIcon);
			m_pPlayButton->setEnabled(true);
			m_pStopButton->setEnabled(true);
			break;
		}
		case Paused:
		{
			m_bPlayState = ControlPanel::Paused;
			m_pPlayButton->setIcon(m_bPlayIcon);
			m_pPlayButton->setEnabled(true);
			m_pStopButton->setEnabled(true);
			break;
		}
		case Stopped:
		{
			m_bPlayState = ControlPanel::Stopped;
			m_pPlayButton->setIcon(m_bPlayIcon);
			m_pPlayButton->setEnabled(true);
			m_pStopButton->setEnabled(false);	
			break;
		}
		case Open:
		{
			m_bPlayState = ControlPanel::Open;
			m_pPlayButton->setIcon(m_bPlayIcon);
			m_pPlayButton->setEnabled(true);
			m_pStopButton->setEnabled(false);
			m_pRewindButton->setEnabled(true);
			m_pForwardButton->setEnabled(true);
			if( m_pSenutu->getCount() > 1)
			{
				m_pRewindButton->setEnabled(true);
			}
		}
		default:
			;
	}
}

void ControlPanel::playMusic(int index)
{
	setPlayState(Playing);
	if(index  == 1)
	{
		m_pRewindButton->setEnabled(false);
	}
	else
	{
		m_pRewindButton->setEnabled(true);
	}
	if(index == m_pSenutu->getCount())
	{
		m_pForwardButton->setEnabled(false);
	}
	else
	{
		m_pForwardButton->setEnabled(true);
	}
	ControlPanel::play(index);
}


void ControlPanel::playORpause()
{
	if( m_bPlayState == ControlPanel::Playing)
	{
		//m_bPlayState = ControlPanel::Paused;
		//m_pPlayButton->setIcon(m_bPlayIcon);
		//m_pPlayButton->setEnabled(true);
		//m_pStopButton->setEnabled(true);
		setPlayState(Paused);
		CAudioCtrl::Pause();
	}
	else if( m_bPlayState == ControlPanel::Paused )
	{
		//m_bPlayState = ControlPanel::Playing;
		//m_pPlayButton->setIcon(m_bPauseIcon);
		//m_pPlayButton->setEnabled(true);
		//m_pStopButton->setEnabled(true);
		setPlayState(Playing);
		CAudioCtrl::Play();
	}
	else if ( m_bPlayState == ControlPanel::Open )
	{
		int currentIndex = m_pSenutu->getCurrentIndex();
		if( currentIndex > 0 )
		{
			int currentIndex = m_pSenutu->getCurrentIndex();
			//m_bPlayState = ControlPanel::Playing;
			//m_pPlayButton->setIcon(m_bPauseIcon);
			//m_pPlayButton->setEnabled(true);
			//m_pStopButton->setEnabled(true);
			setPlayState(Playing);
			ControlPanel::play(currentIndex);
		}
		else
		{
			//TBD
		}

	}
	else if(m_bPlayState == ControlPanel::Stopped)
	{
		//m_bPlayState = ControlPanel::Playing;
		//m_pPlayButton->setIcon(m_bPauseIcon);
		//m_pPlayButton->setEnabled(true);
		//m_pStopButton->setEnabled(true);
		setPlayState(Playing);
		CAudioCtrl::Play();

	}

}

void ControlPanel::stop()
{
	if( m_bPlayState == ControlPanel::Playing || m_bPlayState == ControlPanel::Paused )
	{
		//m_bPlayState = ControlPanel::Stopped;
		//m_pPlayButton->setIcon(m_bPlayIcon);
		//m_pPlayButton->setEnabled(true);
		//m_pStopButton->setEnabled(false);
		setPlayState(Stopped);
		CAudioCtrl::Stop();
	}
}

void ControlPanel::rewind()
{
	CAudioCtrl::Close();
	int currentIndex = m_pSenutu -> getCurrentIndex();
	if(currentIndex <= m_pSenutu->getCount())
	{
		m_pForwardButton ->setEnabled(true);
	}
	if( currentIndex <= 1)
	{
		m_pRewindButton->setEnabled(false);	
	}
	else 
	{
		currentIndex = currentIndex - 1;
		setPlayState(Stopped);
		ControlPanel::play(currentIndex);
	}
}

void ControlPanel::forward()
{
	CAudioCtrl::Close();
	int currentIndex = m_pSenutu ->getCurrentIndex();
	if (currentIndex >= 1)
	{
		m_pRewindButton ->setEnabled(true);
	}
	if( currentIndex >= m_pSenutu->getCount())
	{
		m_pForwardButton->setEnabled(false);	
	}
	else 
	{
		currentIndex = currentIndex + 1;
		setPlayState(Playing);
		ControlPanel::play(currentIndex);
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
	connect(m_pRewindButton,SIGNAL(clicked()),this,SLOT(rewind()));
	connect(m_pForwardButton,SIGNAL(clicked()),this,SLOT(forward()));
}