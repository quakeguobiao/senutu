//Date: 2010.8.21

#ifndef LYRICS_H
#define LYRICS_H

#include <string>
#include <fstream>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

#include "aerror.h"
using namespace std;


class Lyrics: public QObject
{
	Q_OBJECT
public:
	Lyrics(QString artist, QString title);
	~Lyrics() {}//TODO
	
	//determine whether the lyrics of this song has been downloaded 
	//bool IsDownloaded(string artist = m_Artist, string title = m_Title);

	//download the lyrics of a song
	//The file name is .\lyrics\artist - title.lrc.
// 	bool DownloadLyrics(string artist = m_Artist, string title = m_Title);

	//Load a lyrics from a file on the disk. Should be a .lrc file.
	bool LoadLyricsFromFile(const char * fileName);

	//Get a line of lyrics. Usage:
	//GetLyrics(00,38,37) will return 00:38.37's lyrics.
	//Note:Call it after LoadLyricsFromFile(const char*)
	string GetLyrics(int mm, int ss, int ms);
	
	void DownloadFromUrl(QString url);

private slots:
	QString OnDownloadFinished(QNetworkReply * reply);

private:
	Lyrics() {} //do NOT use this!

	//download a certain URL, return a QString
	

//	static const QString m_SearchPath = "http://ttlrcct2.qianqian.com/dll/lyricsvr.dll?sh?Artist={0}&Title={1}&Flags=0";
//	static const QString m_DownloadPath = "http://ttlrcct2.qianqian.com/dll/lyricsvr.dll?dl?Id={0}&Code={1}";

	QString m_Title;  //the title of the song
	QString m_Artist;  //the artist of the song
	QNetworkAccessManager m_NetworkManager;
	QList<QNetworkReply *> m_CurrentDownloads;



};


#endif // LYRICS_H
