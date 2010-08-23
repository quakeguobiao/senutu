#include <QtGui>
#include "ControlPanel.h"
#include "ui_ControlPanel.h"
#include "SenutuStyle.h"
#include "CAudioCtrl.h"
#include "Lyrics.h" 
#include "MainWindow.h"

ControlPanel::ControlPanel( Senutu * parent ):QWidget(parent)
{
	setupUi(this);
	m_pSenutu = parent;
	m_PlayIcon = style()->standardIcon(QStyle::SP_MediaPlay);
	m_PauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
	m_pPlayButton->setIcon(m_PlayIcon);

	m_pStopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));

	m_pRewindButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));

	m_pForwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
	m_pForwardButton->setEnabled(false);

	m_VolumeIcon = style()->standardIcon(QStyle::SP_MediaVolume);
	m_MuteIcon = style()->standardIcon(QStyle::SP_MediaVolumeMuted);
	m_pVolumeState->setIcon(m_VolumeIcon);
	m_pVolumeState->setFlat(true);

	SenutuStyle *style = new SenutuStyle;
	m_pVolumeSlider->setStyle(style);

	m_pPlayInfo->setStyleSheet("border-image:url(:/res/images/screen.png) ; border-width:3px");
	m_pPlayInfo->setText(tr("<center>No media</center>"));
}

void ControlPanel::play()
{
	CAudioCtrl::Open(L"f:\\mssound1.mp3");
	CAudioCtrl::Play();
}

void ControlPanel::stop()
{
	//CAudioCtrl::Stop();


	//for debugging
	Lyrics * temp = new Lyrics("dd","dd");
	//temp->CreateQianQianCode("Nan", "Lonel", 58820);

	temp->SearchLyrics("胡彦斌","情不自禁");
	temp->DownloadLyrics("胡彦斌","情不自禁");

}