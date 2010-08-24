#include <QtGui>
#include <QtGui/QTableWidgetItem>

#include "PlayList.h"
#include "ui_PlayList.h"
#include "MainWindow.h"

PlayList::PlayList(Senutu *parent):QWidget(parent)
{
	setupUi(this);
	m_pSenutu = parent;
	m_pMusicTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pMusicTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	PlayList::createConnections();
}

PlayList::~PlayList()
{

}

void PlayList::AddMusic(TAG & musicInfo)
{
	QTableWidgetItem *titleItem = new QTableWidgetItem;
	titleItem->setText(QString(musicInfo.Title.c_str()));
	titleItem->setFlags(titleItem->flags() ^ Qt::ItemIsEditable);
	Qt::ItemFlags flags = titleItem->flags();
	QTableWidgetItem *artistItem = new QTableWidgetItem;
	artistItem->setText(QString(musicInfo.Artist.c_str()));
	artistItem->setFlags(artistItem->flags() ^ Qt::ItemIsEditable);
	int currentRow = m_pMusicTable->rowCount();
	m_pMusicTable->insertRow(currentRow);
	m_pMusicTable->setItem(currentRow, 0, titleItem);
	m_pMusicTable->setItem(currentRow, 1, artistItem);
}

void PlayList::TableDoubleClicked(int row,int column)
{
	CAudioCtrl::Close();
	int currentIndex = row + 1;
	m_pSenutu->setCurrentIndex(currentIndex);
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

void PlayList::createConnections()
{
	connect(m_pMusicTable, SIGNAL(cellDoubleClicked (int,int)),this, SLOT(TableDoubleClicked(int,int)));
}