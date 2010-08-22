#include "Lyrics.h"

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
		return result;
	}
}
