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
	bool IsDownloaded(string artist, string title){return true;}

	//download the lyrics of a song
	//The file name is .\lyrics\artist - title.lrc.
	bool DownloadLyrics(string artist, string title) {return true;}

	//Load a lyrics from a file on the disk. Should be a .lrc file.
	bool LoadLyricsFromFile(const char * fileName);

	//Get a line of lyrics. Usage:
	//GetLyrics(00,38,37) will return 00:38.37's lyrics.
	//Note:Call it after LoadLyricsFromFile(const char*)
	string GetLyrics(int mm, int ss, int ms);
	
	void DownloadFromUrl(QString url);

private slots:
	//download a certain URL, return a QString
	QString OnDownloadFinished(QNetworkReply * reply);

private:
	Lyrics() {} //do NOT use this!
	
	//get the Unicode of a string, in hexadecimal form
	QString ToQianQianHexString(QString str); 

	static const QString m_SearchPath;
	static const QString m_DownloadPath;

	QString m_Title;  //the title of the song
	QString m_Artist;  //the artist of the song
	QNetworkAccessManager m_NetworkManager;
	QList<QNetworkReply *> m_CurrentDownloads;

};


#endif // LYRICS_H
