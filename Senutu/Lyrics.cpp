#include "Lyrics.h"
#include <QTextCodec>
#include <QtEndian>
#include <QEventLoop>

//helper functions
static long long Conv(int i) {
	long long r = i % 4294967296;
	if (i >= 0 && r > 2147483648)
		r -= 4294967296;

	if (i < 0 && r < 2147483648)
		r += 4294967296;
	return r;
}

static QString ToHexStringByUTF8(QString str) {
	QByteArray byteArray = str.toUtf8();
	QString retval;
	for (int i = 0;i<byteArray.size();i++) {
		QString tmp = QString::number(static_cast<byte>(byteArray[i]),16);
		//convert to 2 digit hex number and store in |tmp|
		while (tmp.size()<2) 
			tmp = "0"+tmp;
		retval+=tmp;
	}
	return retval;
}

static int HexCharToInt(char c) {
	if (isdigit(c))
		return c-'0';
	else 
		return tolower(c) - 'a'+10;
}


//get the Unicode of a string, in hexadecimal form
static QString ToQianQianHexString(QString str)
{
	//TODO: can NOT deal with English characters!
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

//static constants definition 
const QString Lyrics::m_SearchPath =  "http://ttlrcct2.qianqian.com/dll/lyricsvr.dll?sh?Artist=%1&Title=%2&Flags=0";
const QString Lyrics::m_DownloadPath =  "http://ttlrcct2.qianqian.com/dll/lyricsvr.dll?dl?Id=%1&Code=%2";

Lyrics::Lyrics(QString artist, QString title):/*m_Artist(artist), m_Title(title), */m_lyrics("")
{
	connect(&m_NetworkManager, SIGNAL(finished(QNetworkReply*)),SLOT(OnDownloadFinished(QNetworkReply*)));
}

Lyrics::~Lyrics()
{
	QList<QNetworkReply *>::iterator iter;
	for (iter = m_CurrentDownloads.begin() ; iter != m_CurrentDownloads.end() ; ++iter)
		SAFE_DELETE(*iter);
}

bool Lyrics::SearchLyrics(QString artist, QString title)
{
	QString searchURL = m_SearchPath.arg(ToQianQianHexString(artist),ToQianQianHexString(title));
	DownloadFromUrl(searchURL);

	//synchronize
	QEventLoop eventLoop;
	// also dispose the event loop after the reply has arrived
	connect(&m_NetworkManager, SIGNAL(finished(QNetworkReply *)), &eventLoop, SLOT(quit()));
	eventLoop.exec();
	return true;
}

bool Lyrics::DownloadLyrics(QString artist, QString title)
{
	QDomDocument  XML;
	XML.setContent(m_temp);
	QDomNodeList nodeList = XML.elementsByTagName("result");
	nodeList = XML.elementsByTagName("lrc");
	if (nodeList.count() <= 0)
		return false;  //no lyrics found

 	//TODO: let the user choose which one to download.
 	QDomNode node = nodeList.at(0);
 	QString lyrId =node.attributes().namedItem("id").nodeValue();
 	QString Xartist = node.attributes().namedItem("artist").nodeValue();
 	QString Xtitle = node.attributes().namedItem("title").nodeValue();
 	int iLyrId = lyrId.toInt(NULL,10);
	QString downloadURL = m_DownloadPath.arg(lyrId,CreateQianQianCode(Xartist,Xtitle,iLyrId));
	
	DownloadFromUrl(downloadURL);
	//synchronize
	QEventLoop eventLoop;
	// also dispose the event loop after the reply has arrived
	connect(&m_NetworkManager, SIGNAL(finished(QNetworkReply *)), &eventLoop, SLOT(quit()));
	eventLoop.exec();
	//TODO: time limit exceed
	
	return true;
}

void Lyrics::DownloadFromUrl(QString url)
{
	QUrl URL = QUrl::fromEncoded(url.toLocal8Bit());
	QNetworkRequest request(URL);
	QNetworkReply * reply = m_NetworkManager.get(request);
	m_CurrentDownloads.append(reply);
}

void Lyrics::OnDownloadFinished(QNetworkReply * reply)
{
 	if (reply->error()) {
 		atrace_error(qPrintable(reply->errorString()),AR_ERROR_WHILE_DOWNLOADING_LYRICS);  //cannot download from this url
		return ;
	} 
	else 
	{
		QTextCodec * codec = QTextCodec::codecForName("utf8");
		m_temp = codec->toUnicode(reply->readAll());    //the whole page of the whole file
		m_CurrentDownloads.removeAll(reply);
		reply->deleteLater();
	}
}



QString Lyrics::CreateQianQianCode(QString singer,QString title, int lrcId)
{
	// With reference to:
	// 	Filename :QianQianLrcer.cs
	// 	Description :
	// 
	// 	修改记录：
	// 	Created by 吕亮 at 2009-1-15 10:08:16
	// 	算法来源于网络
	QString qqHexStr = ToHexStringByUTF8(singer+title);
	int length = qqHexStr.size() / 2;
	int * song = new int[length];
	for (int i = 0; i < length; i++) {
		song[i] = HexCharToInt(qqHexStr[i*2].toAscii())*16+HexCharToInt(qqHexStr[i*2+1].toAscii());
	}
	int t1 = 0, t2 = 0, t3 = 0;
	t1 = (lrcId & 0x0000FF00) >> 8;
	if ((lrcId & 0x00FF0000) == 0) {
		t3 = 0x000000FF & ~t1;
	} else {
		t3 = 0x000000FF & ((lrcId & 0x00FF0000) >> 16);
	}

	t3 = t3 | ((0x000000FF & lrcId) << 8);
	t3 = t3 << 8;
	t3 = t3 | (0x000000FF & t1);
	t3 = t3 << 8;
	if ((lrcId & 0xFF000000) == 0) {
		t3 = t3 | (0x000000FF & (~lrcId));
	} else {
		t3 = t3 | (0x000000FF & (lrcId >> 24));
	}

	int j = length - 1;
	while (j >= 0) {
		int c = song[j];
		if (c >= 0x80) c = c - 0x100;

		t1 = (int)((c + t2) & 0x00000000FFFFFFFF);
		t2 = (int)((t2 << (j % 2 + 4)) & 0x00000000FFFFFFFF);
		t2 = (int)((t1 + t2) & 0x00000000FFFFFFFF);
		j -= 1;
	}
	j = 0;
	t1 = 0;
	while (j <= length - 1) {
		int c = song[j];
		if (c >= 128) c = c - 256;
		int t4 = (int)((c + t1) & 0x00000000FFFFFFFF);
		t1 = (int)((t1 << (j % 2 + 3)) & 0x00000000FFFFFFFF);
		t1 = (int)((t1 + t4) & 0x00000000FFFFFFFF);
		j += 1;
	}

	int t5 = (int)Conv(t2 ^ t3);
	t5 = (int)Conv(t5 + (t1 | lrcId));
	t5 = (int)Conv(t5 * (t1 | t3));
	t5 = (int)Conv(t5 * (t2 ^ lrcId));

	long long t6 = (long long)t5;
	if (t6 > 2147483648)
		t5 = (int)(t6 - 4294967296);

	//delete all allocated array
	SAFE_DELETE_ARRAY(song);

	return QString::number(t5);
	
}

