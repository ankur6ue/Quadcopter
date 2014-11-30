#ifndef BASICWINDOW_H
#define BASICWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_basicwindow.h"

class BasicWindow : public QMainWindow
{
	Q_OBJECT

public:
	BasicWindow(QWidget *parent = 0);
	~BasicWindow();

private:
	Ui::BasicWindowClass ui;
};

#endif // BASICWINDOW_H
