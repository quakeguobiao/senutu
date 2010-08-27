#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtGui/QIcon>
#include <QtGui/QSlider>
#include <QtGui/QLabel>

#include "ui_ControlPanel.h"

class Senutu;

class ControlPanel:public QWidget,public Ui::ControlPanel
{
	Q_OBJECT
public:
	enum PlayState{Playing,Paused,Stopped,Open};
	ControlPanel(Senutu* parent);
	void setPlayState(PlayState playstate);
	void timerEvent(QTimerEvent *event);

public slots:
	void playMusic(int index);

private slots:
	void playORpause();
	void stop();
	void rewind();
	void forward();
	void SeekSliderMoved(int move);
	//void setvolume();
	//void setposition();

private:
	void createConnections();
	void play(int index);

	Senutu* m_pSenutu;
	int m_bTimeID;
	QIcon m_bPlayIcon;
	QIcon m_bPauseIcon;
	QIcon m_bVolumeIcon;
	QIcon m_bMuteIcon;
	ControlPanel::PlayState m_bPlayState;

};


#endif // CONTROLPANEL_H

