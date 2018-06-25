#include "smartmonitorsystem.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SmartMonitorSystem w;
	w.show();
	return a.exec();
}
