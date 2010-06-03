#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QPushButton>
#include <QIcon>
#include <QSlider>
#include <QLabel>
#include <QGridLayout>

class ControlPanel:public QWidget{
	Q_OBJECT

public:
	ControlPanel(QWidget* parent = 0);
	//~ControlPanel();
	void SetupControlPanel();
	QWidget* GetWidget();

//private slots:
//	void play();
//	void pause();
//	void stop();
//	void rewind();
//	void setvolume();
//	void setposition();

private:
	QIcon m_PlayIcon;
	QIcon m_PauseIcon;
	QIcon m_VolumeIcon;
	QIcon m_MuteIcon;
	QPushButton *m_pPlayButton;
    QPushButton *m_pRewindButton;
    QPushButton *m_pForwardButton;
	QPushButton *m_pStopButton;
	QPushButton *m_pVolumeState;  
	QLabel *m_pPlayInfo;                                   //display the playing info                    
	QSlider *m_pSeekSlider;
	QSlider *m_pVolumeSlider;
	QWidget *m_pControlPanelWidget;

};


#endif // CONTROLPANEL_H
