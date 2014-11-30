#include "qttestapplication.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QtTestApplication w;
	w.show();
	return a.exec();
}
