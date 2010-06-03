/********************************************************************************
** Form generated from reading UI file 'senutu.ui'
**
** Created: Thu Jun 3 23:20:47 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SENUTU_H
#define UI_SENUTU_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SenutuClass
{
public:
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *SenutuClass)
    {
        if (SenutuClass->objectName().isEmpty())
            SenutuClass->setObjectName(QString::fromUtf8("SenutuClass"));
        SenutuClass->resize(600, 400);
        centralWidget = new QWidget(SenutuClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        SenutuClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(SenutuClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 24));
        SenutuClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(SenutuClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        SenutuClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(SenutuClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        SenutuClass->setStatusBar(statusBar);

        retranslateUi(SenutuClass);

        QMetaObject::connectSlotsByName(SenutuClass);
    } // setupUi

    void retranslateUi(QMainWindow *SenutuClass)
    {
        SenutuClass->setWindowTitle(QApplication::translate("SenutuClass", "Senutu", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SenutuClass: public Ui_SenutuClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SENUTU_H
