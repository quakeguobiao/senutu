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

void PlayList::AddMusic(const TAG & musicInfo)
{
	QTableWidgetItem *titleItem = new QTableWidgetItem;
	titleItem->setText(QString(musicInfo.Title.c_str()));
	titleItem->setFlags(titleItem->flags() ^ Qt::ItemIsEditable);
	QTableWidgetItem *artistItem = new QTableWidgetItem;
	artistItem->setText(QString(musicInfo.Artist.c_str()));
	artistItem->setFlags(artistItem->flags() ^ Qt::ItemIsEditable);
	QTableWidgetItem *albumnItem = new QTableWidgetItem;
	albumnItem->setText(QString(musicInfo.Album.c_str()));
	albumnItem->setFlags(albumnItem->flags() ^ Qt::ItemIsEditable);
	QTableWidgetItem *genreItem = new QTableWidgetItem;
	genreItem->setText(QString(musicInfo.Genre.c_str()));
	genreItem->setFlags(genreItem->flags() ^ Qt::ItemIsEditable);
	QTableWidgetItem *timeItem = new QTableWidgetItem;
	timeItem->setText(QString(musicInfo.TotalTime.c_str()));
	genreItem->setFlags(timeItem->flags() ^ Qt::ItemIsEditable);
	int currentRow = m_pMusicTable->rowCount();
	m_pMusicTable->insertRow(currentRow);
	m_pMusicTable->setItem(currentRow, 0, titleItem);
	m_pMusicTable->setItem(currentRow, 1, artistItem);
	m_pMusicTable->setItem(currentRow, 2, albumnItem);
	m_pMusicTable->setItem(currentRow, 3, genreItem);
	m_pMusicTable->setItem(currentRow, 4, timeItem);
}

void PlayList::TableDoubleClicked(int row,int column)
{
	CAudioCtrl::Close();
	int currentIndex = row + 1;
	m_pSenutu->setCurrentIndex(currentIndex);
	//QStringList QFileNames = m_pSenutu->getMusicList();
	//QString QFileName = QFileNames[currentIndex-1];
	//std::string sFN = QFileName.toStdString();
	//char* strFileName = const_cast<char*>(sFN.c_str());
	//int length = MultiByteToWideChar(CP_ACP, 0, strFileName, -1, NULL, 0); 
	//wchar_t * fileName = new wchar_t[length+1]; 
	//MultiByteToWideChar(CP_ACP, 0, strFileName, -1, fileName, length);   
	//CAudioCtrl::Open(fileName);
	//CAudioCtrl::Play();
	//SAFE_DELETE_ARRAY(fileName);
	emit mediaSourceChanged(currentIndex);

}

void PlayList::TableClicked(int row, int column)
{
	int currentIndex = row + 1;
	m_pSenutu->setCurrentIndex(currentIndex);
	emit currentIndexChanged();
}

void PlayList::createConnections()
{
	connect(m_pMusicTable, SIGNAL(cellDoubleClicked (int,int)),this, SLOT(TableDoubleClicked(int,int)));
	connect(m_pMusicTable, SIGNAL(cellClicked (int,int)),this,SLOT(TableClicked(int,int)));
}