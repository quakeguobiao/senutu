//// console main function
//#include <QtGui/QApplication>
//#include <QtCore/QTextCodec>
//#include "MainWindow.h"
//#include <QtCore\QString>
//#include <QTCore\QStdWString>
//
//
//
//int main(int argc, char *argv[])
//{
//QString tmp = QString(argv[1]);
//
//CAudioCtrl::Open(const_cast<LPWSTR>(tmp.toStdWString().c_str()));
//TAG ret = CAudioCtrl::GetTag();
//
//
//
//
//return 0;
//}
//qt main function

#include <QtGui/QApplication>
#include <QTextCodec>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//QTextCodec::setCodecForTr( QTextCodec::codecForLocale());
	//QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
	Senutu w;
	w.show();
	return a.exec();
}
