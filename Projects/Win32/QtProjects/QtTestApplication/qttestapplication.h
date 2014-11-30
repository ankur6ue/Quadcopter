#ifndef QTTESTAPPLICATION_H
#define QTTESTAPPLICATION_H

#include <QtWidgets/QMainWindow>
#include "ui_qttestapplication.h"

class QtTestApplication : public QMainWindow
{
	Q_OBJECT

public:
	QtTestApplication(QWidget *parent = 0);
	~QtTestApplication();

private:
	Ui::QtTestApplicationClass ui;
};

#endif // QTTESTAPPLICATION_H
