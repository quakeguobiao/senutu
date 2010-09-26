#include <QtGui>
#include "ControlPanel.h"
#include "ui_ControlPanel.h"
#include "SenutuStyle.h"
#include "CAudioCtrl.h"
#include "Lyrics.h" 
#include "MainWindow.h"
#include "aerror.h"

ControlPanel::ControlPanel( Senutu * parent ):QWidget(parent),
_isSliderDragging(false)
{
	setupUi(this);
	m_pSenutu = parent;
	m_bTimeID = 0;
	m_bVolume = 2;
	m_bIsMute = false;
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
	m_pVolumeSlider->setRange(0,10);
	m_pVolumeSlider->setValue(m_bVolume);
	m_pPlayInfo->setStyleSheet("border-image:url(:/res/images/screen.png) ; border-width:3px");
	m_pPlayInfo->setText(tr("<center>No media</center>"));

	ControlPanel::createConnections();
	//set up the sync loop
	QTimer * syncTimer = new QTimer(this);
	syncTimer->start(20);
	connect(syncTimer,SIGNAL(timeout()),this,SLOT(sync()));
}

void ControlPanel::setPlayState(PlayState playstate)
{
	m_bPlayState = playstate;
	switch (playstate){
		case Playing:
		{
			if (!m_bTimeID)
				m_bTimeID = QObject::startTimer(1000/20);
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

void ControlPanel::timerEvent(QTimerEvent *event)
{
	
	if (event->timerId() == m_bTimeID) 
	{
		if(m_bPlayState == ControlPanel::Playing )
		{
			if (!_isSliderDragging)
				m_pSeekSlider->setValue(CAudioCtrl::GetCurTime());
			
		}
	}
	QObject::timerEvent(event);

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

void ControlPanel::currentIndexChanged()
{
	setPlayState(ControlPanel::Open);
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
			CAudioCtrl::Close();
			//int currentIndex = m_pSenutu->getCurrentIndex();
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
		m_pSeekSlider->setValue(0);
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
	if( currentIndex <= 2)
	{
		m_pRewindButton->setEnabled(false);	
	}
	if( currentIndex >1 )
		currentIndex = currentIndex - 1;
	m_pSenutu->setCurrentIndex(currentIndex);
	setPlayState(Stopped);
	ControlPanel::play(currentIndex);
	emit rewindOrForward(currentIndex);
}

void ControlPanel::forward()
{
	CAudioCtrl::Close();
	int currentIndex = m_pSenutu ->getCurrentIndex();
	if (currentIndex >= 1)
	{
		m_pRewindButton ->setEnabled(true);
	}
	if( currentIndex >= m_pSenutu->getCount()-1)
	{
		m_pForwardButton->setEnabled(false);	
	}
	if(currentIndex < m_pSenutu->getCount())
		currentIndex = currentIndex + 1;
	m_pSenutu->setCurrentIndex(currentIndex);
	setPlayState(Stopped);
	ControlPanel::play(currentIndex);
	emit rewindOrForward(currentIndex);
}

void ControlPanel::SeekSliderMoved(int move)
{
	m_sliderPos = move;
	/*
	if( m_bPlayState == ControlPanel::Playing)
	{
		CAudioCtrl::SetCurTime(move);
	}*/
}

void ControlPanel::volumeORmute()
{
	if(m_bIsMute == false)
	{
		m_bIsMute = true;
		m_pVolumeState->setIcon(m_bMuteIcon);
		m_bVolume = CAudioCtrl::GetVolume();
		CAudioCtrl::SetVolume(0);
	}
	else
	{
		m_bIsMute = false;
		m_pVolumeState->setIcon(m_bVolumeIcon);
		CAudioCtrl::SetVolume(m_bVolume);
	}
}

void ControlPanel::volumeSliderMoved(int move)
{
	CAudioCtrl::SetVolume(move);
	m_bVolume = move;
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
	int time = CAudioCtrl::GetFullTime();
	m_pSeekSlider->setRange(0,time);
	CAudioCtrl::Play();
	CAudioCtrl::SetVolume(m_bVolume);
	SAFE_DELETE_ARRAY(fileName);
}

void ControlPanel::SeekSliderReleased() {
	_isSliderDragging=false;
	CAudioCtrl::SetCurTime(m_sliderPos);
}

void ControlPanel::createConnections()
{
	connect(m_pPlayButton,SIGNAL(clicked()),this,SLOT(playORpause()));
	connect(m_pStopButton,SIGNAL(clicked()),this,SLOT(stop()));
	connect(m_pRewindButton,SIGNAL(clicked()),this,SLOT(rewind()));
	connect(m_pForwardButton,SIGNAL(clicked()),this,SLOT(forward()));
	connect(m_pVolumeState,SIGNAL(clicked()),this,SLOT(volumeORmute()));
	connect(m_pSeekSlider,SIGNAL(sliderMoved(int)),this,SLOT(SeekSliderMoved(int)));
	connect(m_pSeekSlider,SIGNAL(sliderReleased()),this,SLOT(SeekSliderReleased()));
	connect(m_pSeekSlider,SIGNAL(sliderPressed()),this,SLOT(SeekSliderPressed()));
	connect(m_pVolumeSlider,SIGNAL(sliderMoved(int)),this,SLOT(volumeSliderMoved(int)));
}

void ControlPanel::SeekSliderPressed()
{
	_isSliderDragging=true;
}

void ControlPanel::sync()
{
	CAudioCtrl::Sync();
}