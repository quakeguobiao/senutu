// // console main function
// #include <QtGui/QApplication>
// #include <QTextCodec>
// #include "MainWindow.h"
// #include <QtCore\QString>
// #include <QTCore\QStdWString>
// #include <vector>
// 
// 
// int main(int argc, char *argv[])
// {
// 	QString tmp = QString(argv[1]);
// 	CAudioCtrl::Init();
// 	CAudioCtrl::Open(const_cast<LPWSTR>(tmp.toStdWString().c_str()));
// 	CAudioCtrl::Play();
// 	/*std::vector<float> freqs = CAudioCtrl::GetEQFreqs();
//     int index = 0;
// 	while (1) {
// 		Sleep(200);
// 		for (int i = 0;i<freqs.size();i++)
// 			cout<<
// 	}*/
// 	/*while (1) {
// 		cout<<CAudioCtrl::GetFreqGain(0)<<"\r";
// 		if (kbhit()) {
// 			char c = getc();
// 			if (c=='w')
// 				CAudioCtrl::SetFreqGain()
// 			if (c=='s')
// 		}
// 	}*/
// 	while (CAudioCtrl::isPlaying()){
// 		Sleep(200);
// 	}
// 	
// 	return 0;
// }


 

#include <QtGui/QApplication>
#include <QTextCodec>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
	Senutu w;
	w.show();
	return a.exec();
}
