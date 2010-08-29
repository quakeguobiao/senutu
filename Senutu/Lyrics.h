////Date: 2010.8.21
//
//#ifndef LYRICS_H
//#define LYRICS_H
//
//#include <string>
//#include <fstream>
//#include <QDomDocument>
//#include <QNetworkAccessManager>
//#include <QNetworkRequest>
//#include <QNetworkReply>
//#include <QUrl>
//
//#include "aerror.h"
//using namespace std;
//
//
//class Lyrics: public QObject
//{
//	Q_OBJECT
//public:
//	Lyrics(QString artist, QString title);
//	~Lyrics();
//	
//	//determine whether the lyrics of this song has been downloaded 
//	bool IsDownloaded(string artist, string title){return false;}
//
//	//download the lyrics of a song
//	//The file name is .\lyrics\artist - title.lrc.
//	bool DownloadLyrics(QString artist, QString title);
//
//	bool SearchLyrics(QString artist, QString title);
//	
//	//Load a lyrics from a file on the disk. Should be a .lrc file.
//	bool LoadLyricsFromFile(const char * fileName);
//
//	//Get a line of lyrics. Usage:
//	//GetLyrics(00,38,37) will return 00:38.37's lyrics.
//	//Note:Call it after LoadLyricsFromFile(const char*)
//	string GetLyrics(int mm, int ss, int ms);
//	
//
//	//get a qianqian hashcode
//	QString CreateQianQianCode(QString singer,QString title, int lrcId);
//
//private slots:
//	//download a certain URL, modifies *this
//	void OnDownloadFinished(QNetworkReply * reply);
//
//private:
//	Lyrics() {} //do NOT use this!
//	
//	static const QString m_SearchPath;
//	static const QString m_DownloadPath;
//
//	QString m_temp;  //store the current downloaded string
//	QString m_lyrics;  //the whole lyrics of the current song
//	
//	void DownloadFromUrl(QString url);  //helper function
//
//	
//	QNetworkAccessManager m_NetworkManager;
//	QList<QNetworkReply *> m_CurrentDownloads;
//
//};
//
//
//#endif // LYRICS_H
