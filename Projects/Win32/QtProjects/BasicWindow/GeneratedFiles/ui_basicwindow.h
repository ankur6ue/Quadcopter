/********************************************************************************
** Form generated from reading UI file 'basicwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BASICWINDOW_H
#define UI_BASICWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BasicWindowClass
{
public:
    QWidget *centralWidget;
    QRadioButton *radioButton;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *BasicWindowClass)
    {
        if (BasicWindowClass->objectName().isEmpty())
            BasicWindowClass->setObjectName(QStringLiteral("BasicWindowClass"));
        BasicWindowClass->resize(600, 400);
        centralWidget = new QWidget(BasicWindowClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        radioButton = new QRadioButton(centralWidget);
        radioButton->setObjectName(QStringLiteral("radioButton"));
        radioButton->setGeometry(QRect(120, 80, 95, 20));
        BasicWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(BasicWindowClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 26));
        BasicWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(BasicWindowClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        BasicWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(BasicWindowClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        BasicWindowClass->setStatusBar(statusBar);

        retranslateUi(BasicWindowClass);

        QMetaObject::connectSlotsByName(BasicWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *BasicWindowClass)
    {
        BasicWindowClass->setWindowTitle(QApplication::translate("BasicWindowClass", "BasicWindow", 0));
        radioButton->setText(QApplication::translate("BasicWindowClass", "RadioButton", 0));
    } // retranslateUi

};

namespace Ui {
    class BasicWindowClass: public Ui_BasicWindowClass {};
} // namespace Ui

QT_ENpNAMESPACE

#endif // UI_BASICWINDOW_H
