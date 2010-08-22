#include <QtGui>
#include <QtGui/QTableWidgetItem>

#include "PlayList.h"
#include "ui_PlayList.h"

PlayList::PlayList(QWidget *parent):QWidget(parent)
{
	setupUi(this);
}

void PlayList::AddMusic(TAG & musicInfo)
{
	QTableWidgetItem *titleItem = new QTableWidgetItem;
	titleItem->setText(QString(musicInfo.Title.c_str()));
	titleItem->setFlags(titleItem->flags() ^ Qt::ItemIsEditable);
	QTableWidgetItem *artistItem = new QTableWidgetItem;
	artistItem->setText(QString(musicInfo.Artist.c_str()));
	artistItem->setFlags(artistItem->flags() ^ Qt::ItemIsEditable);
	int currentRow = m_pMusicTable->rowCount();
	m_pMusicTable->insertRow(currentRow);
	m_pMusicTable->setItem(currentRow, 0, titleItem);
	m_pMusicTable->setItem(currentRow, 1, artistItem);
}