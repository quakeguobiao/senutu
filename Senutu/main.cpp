#include <QtGui/QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Senutu w;
	w.show();
	return a.exec();
}
