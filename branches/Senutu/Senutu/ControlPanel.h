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
	ControlPanel(Senutu* parent );

private slots:
	void play();
	//void pause();
	void stop();
	//void rewind();
	//void setvolume();
	//void setposition();

private:
	//void createConnections();

	Senutu* m_pSenutu;
	QIcon m_PlayIcon;
	QIcon m_PauseIcon;
	QIcon m_VolumeIcon;
	QIcon m_MuteIcon;

};


#endif // CONTROLPANEL_H

