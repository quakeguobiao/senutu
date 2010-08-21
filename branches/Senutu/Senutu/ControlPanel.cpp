#include <QtGui>
#include "ControlPanel.h"
#include "CAudioCtrl.h"
ControlPanel::ControlPanel( QWidget * parent )
{
	//Buttons
	QSize ButtonSize;
    ButtonSize.setHeight(28);
	ButtonSize.setWidth(34);

	m_pRewindButton = new QPushButton(parent);
    m_pRewindButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
	
    m_pForwardButton = new QPushButton(parent);
    m_pForwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    m_pForwardButton->setEnabled(false);

	m_pPlayButton = new QPushButton(parent);
    m_PlayIcon = style()->standardIcon(QStyle::SP_MediaPlay);
	m_PauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
    m_pPlayButton->setIcon(m_PlayIcon);

	m_pStopButton = new QPushButton(parent);
	m_pStopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));

	m_pRewindButton->setFixedSize(ButtonSize);
    m_pForwardButton->setFixedSize(ButtonSize);
    m_pPlayButton->setFixedSize(ButtonSize);
	m_pStopButton->setFixedSize(ButtonSize);

	m_pVolumeState = new QPushButton(parent);
	m_VolumeIcon = style()->standardIcon(QStyle::SP_MediaVolume);
	m_MuteIcon = style()->standardIcon(QStyle::SP_MediaVolumeMuted);
	m_pVolumeState->setIcon(m_VolumeIcon);
	m_pVolumeState->setFixedHeight(20);
	m_pVolumeState->setFixedWidth(28);
	m_pVolumeState->setFlat(true);
	//Sliders
	m_pSeekSlider = new QSlider(Qt::Horizontal,parent);
	m_pVolumeSlider = new QSlider(Qt::Horizontal,parent);
	m_pVolumeSlider->setFixedWidth(100);
	m_pSeekSlider->setMaximumWidth(310);
	//Label
	m_pPlayInfo = new QLabel(parent);
	m_pPlayInfo->setStyleSheet("border-image:url(:Senutu/Resources/screen.png) ; border-width:3px");
	m_pPlayInfo->setText(tr("<center>No media</center>"));
	m_pPlayInfo->setMinimumWidth(300);
	//Layout

	m_pControlPanelWidget = new QWidget(parent);
}

void ControlPanel::SetupControlPanel()
{
	QGridLayout *ControlPanelLayout = new QGridLayout();
	ControlPanelLayout->addWidget(m_pStopButton,0,0);
	ControlPanelLayout->addWidget(m_pRewindButton,0,1);
	ControlPanelLayout->addWidget(m_pPlayButton,0,2);
	ControlPanelLayout->addWidget(m_pForwardButton,0,3);
	ControlPanelLayout->addWidget(m_pVolumeState,0,4);
	ControlPanelLayout->addWidget(m_pVolumeSlider,0,5);
	ControlPanelLayout->addWidget(m_pPlayInfo,0,6,2,1);
	ControlPanelLayout->addWidget(m_pSeekSlider,1,0,1,6);
	ControlPanelLayout->setColumnStretch(6,1);
	m_pControlPanelWidget->setLayout(ControlPanelLayout);

	
	connect(m_pPlayButton,SIGNAL(clicked()), this, SLOT(play()));
	connect(m_pStopButton,SIGNAL(clicked()), this, SLOT(stop()));
	
}

QWidget* ControlPanel::GetWidget()
{
	return m_pControlPanelWidget;
}

void ControlPanel::play()
{
	CAudioCtrl::Open(L"f:\\mssound1.mp3");
	CAudioCtrl::Play();
}

void ControlPanel::stop()
{
	CAudioCtrl::Stop();
}