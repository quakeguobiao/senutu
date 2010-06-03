#include <QtGui>
#include "PlayList.h"

PlayList::PlayList(QWidget* parent)
{
	m_pMusicTable = new QTableWidget(0,4,parent);
	QStringList headers;
	headers << tr("Title") <<tr("Time")<< tr("Artist") << tr("Album");
	m_pMusicTable->setHorizontalHeaderLabels(headers);
	m_pMusicTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pMusicTable->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void PlayList::AddPlayList()
{
}

QTableWidget* PlayList::GetWidget()
{
	return m_pMusicTable;
}