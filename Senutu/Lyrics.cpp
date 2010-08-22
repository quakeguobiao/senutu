#include "Lyrics.h"
#include <QtEndian>
//helper functions
static long long Conv(int i) {
	long long r = i % 4294967296;
	if (i >= 0 && r > 2147483648)
		r = r - 4294967296;

	if (i < 0 && r < 2147483648)
		r = r + 4294967296;
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
//		ToQianQianHexString("ºúÑå±ó");
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



QString Lyrics::CreateQianQianCode( QString singer,QString title, int lrcId )
{
	
	QString qqHexStr = ToHexStringByUTF8(singer+title);
	int length = qqHexStr.size() / 2;
	int * song = new int[length];
	for (int i = 0; i < length; i++) {
		//song[i] = int.Parse(qqHexStr.Substring(i * 2, 2), System.Globalization.NumberStyles.HexNumber);
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

