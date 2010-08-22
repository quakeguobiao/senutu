#include "Lyrics.h"
#include <QtEndian>

const QString Lyrics::m_SearchPath =  "http://ttlrcct2.qianqian.com/dll/lyricsvr.dll?sh?Artist=1%&Title=2%&Flags=0";
const QString Lyrics::m_DownloadPath =  "http://ttlrcct2.qianqian.com/dll/lyricsvr.dll?dl?Id=1%&Code=2%";

Lyrics::Lyrics(QString artist, QString title):m_Artist(artist), m_Title(title) 
{
	connect(&m_NetworkManager, SIGNAL(finished(QNetworkReply*)),SLOT(OnDownloadFinished(QNetworkReply*)));
}

//bool Lyrics::DownloadLyrics(string artist, string title)
//{
//	return true;
//}

void Lyrics::DownloadFromUrl(QString url)
{
	QUrl URL = QUrl::fromEncoded(url.toLocal8Bit());
	QNetworkRequest request(URL);
	QNetworkReply * reply = m_NetworkManager.get(request);
	m_CurrentDownloads.append(reply);
}

QString Lyrics::OnDownloadFinished(QNetworkReply * reply)
{
 	if (reply->error()) {
 		atrace_error(qPrintable(reply->errorString()),AR_ERROR_WHILE_DOWNLOADING_LYRICS);  //cannot download from this url
 		return m_Title;
	} 
	else 
	{
		QString result(reply->readAll());    //the whole page of the whole file
		m_CurrentDownloads.removeAll(reply);
		reply->deleteLater();
		ToQianQianHexString("ºúÑå±ó");
		return result;
	}
}

QString Lyrics::ToQianQianHexString(QString str)
{
	wchar_t WideCharStr[200];  //assume that the string is no longer than 200 characters
	str.toWCharArray(WideCharStr);
	QString result;
	for (int i = 0 ; i < str.count() ; ++i){
		int x = qFromBigEndian(WideCharStr[i]);   //reverse: from litter-endian to big-endian
		char temp[10];
		sprintf(temp,"%04X",x);   //convert to hexadecimal, pad left 4 digits
		result += temp;
	}
	return result;

}