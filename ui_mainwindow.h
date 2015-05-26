/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionAbout_SDM;
    QAction *actionPause_All;
    QAction *actionStop_All;
    QAction *actionAdd;
    QAction *actionRemoe;
    QAction *actionExit;
    QAction *actionPause_All_2;
    QAction *actionStop_All_2;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuDownlaods;
    QMenu *menuView;
    QMenu *menuHelp;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(744, 346);
        actionAbout_SDM = new QAction(MainWindow);
        actionAbout_SDM->setObjectName(QString::fromUtf8("actionAbout_SDM"));
        actionPause_All = new QAction(MainWindow);
        actionPause_All->setObjectName(QString::fromUtf8("actionPause_All"));
        actionStop_All = new QAction(MainWindow);
        actionStop_All->setObjectName(QString::fromUtf8("actionStop_All"));
        actionAdd = new QAction(MainWindow);
        actionAdd->setObjectName(QString::fromUtf8("actionAdd"));
        actionRemoe = new QAction(MainWindow);
        actionRemoe->setObjectName(QString::fromUtf8("actionRemoe"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionPause_All_2 = new QAction(MainWindow);
        actionPause_All_2->setObjectName(QString::fromUtf8("actionPause_All_2"));
        actionStop_All_2 = new QAction(MainWindow);
        actionStop_All_2->setObjectName(QString::fromUtf8("actionStop_All_2"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 744, 19));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuDownlaods = new QMenu(menuBar);
        menuDownlaods->setObjectName(QString::fromUtf8("menuDownlaods"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuDownlaods->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionAdd);
        menuFile->addAction(actionRemoe);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuDownlaods->addAction(actionPause_All_2);
        menuDownlaods->addAction(actionStop_All_2);
        menuView->addAction(actionPause_All);
        menuView->addAction(actionStop_All);
        menuView->addSeparator();
        menuHelp->addAction(actionAbout_SDM);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        actionAbout_SDM->setText(QApplication::translate("MainWindow", "About SDM", 0, QApplication::UnicodeUTF8));
        actionPause_All->setText(QApplication::translate("MainWindow", "Pause All", 0, QApplication::UnicodeUTF8));
        actionStop_All->setText(QApplication::translate("MainWindow", "Stop All", 0, QApplication::UnicodeUTF8));
        actionAdd->setText(QApplication::translate("MainWindow", "Add", 0, QApplication::UnicodeUTF8));
        actionRemoe->setText(QApplication::translate("MainWindow", "Remove", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("MainWindow", "Exit", 0, QApplication::UnicodeUTF8));
        actionPause_All_2->setText(QApplication::translate("MainWindow", "Pause All", 0, QApplication::UnicodeUTF8));
        actionStop_All_2->setText(QApplication::translate("MainWindow", "Stop All", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
        menuDownlaods->setTitle(QApplication::translate("MainWindow", "Downloads", 0, QApplication::UnicodeUTF8));
        menuView->setTitle(QApplication::translate("MainWindow", "View", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
