/*
 * Copyright 2015 Ashish Bansal<bansal.ashish096@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "mainwindow.h"
#include "download.h"
#include "adddialog.h"
#include "downloadinfodialog.h"
#include "startdownload.h"
#include "singletonfactory.h"
#include "updater.h"
#include "debug.h"
#include "localserver.h"
#include <ui_downloadinfodialog.h>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTableView>
#include <QtWidgets>
#include <QPair>
#include <QPointer>
#include <QScopedPointer>
#include <QDateTime>
#include <QTreeWidgetItemIterator>
#include <QMessageBox>
#include <QStandardPaths>
#include <QSystemTrayIcon>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qInstallMessageHandler(Debug::debugStyle);
//    LocalServer *server = new LocalServer();
//    Updater *up = new Updater(this);
    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        qDebug() << "SystemTrayAvailable";
        QSystemTrayIcon *trayIcon = new QSystemTrayIcon(QIcon(":icons/download-icon.png"), this);
        QMenu *menu = new QMenu(this);
        trayIcon->setContextMenu(menu);
        menu->addSection("adfasd");
        QAction* restoreAction = new QAction(tr("&Restore"), this);
        menu->addAction(restoreAction);
        trayIcon->setVisible(true);
        trayIcon->show();
        connect(trayIcon, &QSystemTrayIcon::activated, this, &QWidget::showNormal);
        connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));
        trayIcon->showMessage("SDM", "SDM has been started", QSystemTrayIcon::Information, 1 * 1000);
    }
    m_model = SingletonFactory::instanceFor<DownloadModel>();

    m_downloadView = new DownloadView(this);
    m_downloadView->setModel(m_model);
    m_downloadView->loadViewSettings();

    connect(m_downloadView->horizontalHeader(), &QHeaderView::sectionPressed, this, &MainWindow::saveHeaderState);
    QVBoxLayout *layout = new QVBoxLayout();
    QLineEdit *ledit = new QLineEdit();
    ledit->setPlaceholderText("Search Box");
    layout->addWidget(ledit);
    layout->addWidget(m_downloadView);
    setCentralWidget(new QWidget);
    centralWidget()->setLayout(layout);
    showMaximized();

    // Create common actions
    QAction *addDownload = new QAction(QIcon(":icons/download-icon.png"), tr("Add Download"), this);
    QAction *removeDownload = new QAction(QIcon(":icons/remove.png"), tr("Delete Download"), this);
    QAction *startDownload = new QAction(QIcon(":icons/play-icon.png"), tr("Start/Resume Download"), this);
    QAction *stopDownload = new QAction(QIcon(":icons/stop-2-icon.png"), tr("Stop Download"), this);
    QAction *restartDownload = new QAction(QIcon(":icons/reload-icon.png"), tr("Restart Download"), this);
//   restartDownload->setEnabled(false);

    // File menu
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(addDownload);
    fileMenu->addAction(removeDownload);
    fileMenu->addSeparator();
    fileMenu->addAction(QIcon(":/icons/exit.png"), tr("Hide to System Tray"), this, SLOT(close()));
    fileMenu->addAction(QIcon(":/icons/exit.png"), tr("Quit"), this, SLOT(exit()));

    // Help menu
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About"), this, SLOT(about()));

    // Top toolbar
    QToolBar *topBar = new QToolBar(tr("Tools"));
    addToolBar(Qt::TopToolBarArea, topBar);
    topBar->setMovable(false);

    topBar->addAction(addDownload);
    topBar->addAction(removeDownload);
    topBar->addSeparator();
    topBar->addAction(startDownload);
    topBar->addAction(stopDownload);
    topBar->addAction(restartDownload);
    topBar->setIconSize(QSize(50,50));

    loadDownloads();

    connect(addDownload, &QAction::triggered, this, &MainWindow::onActionAddTriggered);
    connect(startDownload, &QAction::triggered, this, &MainWindow::onActionResumeTriggered);
    connect(restartDownload, &QAction::triggered, this, &MainWindow::onActionRestartTriggered);
    connect(stopDownload, &QAction::triggered, this, &MainWindow::onActionStopTriggered);
    connect(removeDownload, &QAction::triggered, this, &MainWindow::onActionRemoveTriggered);

    QMetaObject::invokeMethod(this, "loadSettings", Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow Exit";
}

void MainWindow::exit()
{
    QMessageBox::StandardButton ans;
    ans = QMessageBox::question(this, "Close Confirmation?",
                                "Are you sure you want to exit?", QMessageBox::Yes|QMessageBox::No);
    if (QMessageBox::Yes == ans) {
        this->setAttribute(Qt::WA_DeleteOnClose);
        this->close();
    }
}

void MainWindow::loadDownloads()
{
    m_model->readDatabase();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
    hide();
}

QList< int > MainWindow::getSelectedItemIds()
{
    QList< int > ids;
    QItemSelectionModel *selectedItemModel = m_downloadView->selectionModel();
    auto items = selectedItemModel->selectedRows(Enum::DownloadAttributes::DatabaseId);
    foreach(auto item, items) {
        qint64 id = m_model->data(item, Qt::DisplayRole).toInt();
        ids.append(id);
    }
    return ids;
}

void MainWindow::onActionAddTriggered()
{
    AddDialog addDialog;
    addDialog.setWindowTitle("Add Download");
    addDialog.setModal(true);
    connect(&addDialog, &AddDialog::showDownloadDialog, this, &MainWindow::showDownloadDialog);
    addDialog.exec();
}

void MainWindow::onActionResumeTriggered()
{
    QList< int > ids = getSelectedItemIds();
    int restartCount = ids.size();
    if (restartCount == 0) {
        return;
    }

    int resumeNotSupportedCount = 0;
    foreach(int id, ids) {
        if (!checkResumeSupported(id)) {
            resumeNotSupportedCount += 1;
        }
    }

    if (resumeNotSupportedCount == 0) {
        foreach(int id, ids) {
            StartDownload *newDownload = new StartDownload(id);
            m_downloads.insert(id, newDownload);
            newDownload->startDownload();
        }
    } else {
        QMessageBox::StandardButton ans;
        QString message = "Some of the downloads you selected cannot be resumed. \n"
                          "Do you want to restart them ?";
        ans = QMessageBox::question(this, "Restart Confirmation?",
                                    message, QMessageBox::Yes|QMessageBox::No);
        if (ans == QMessageBox::Yes) {
            foreach(int id, ids) {
                if (checkResumeSupported(id)) {
                    resumeDownload(id);
                } else {
                    restartDownload(id);
                }
            }
        } else {
            foreach(int id, ids) {
                if (checkResumeSupported(id)) {
                    resumeDownload(id);
                }
            }
        }
    }
}

void MainWindow::resumeDownload(int id)
{
    auto it = m_downloads.find(id);
    if (it != m_downloads.end()) {
        it.value()->startDownload();
    }
}

bool MainWindow::checkResumeSupported(int id)
{
    const DownloadAttributes *properties = m_model->getDetails(id);
    if (properties->resumeCapability == Enum::SDM::ResumeSupported) {
        return true;
    }
    return false;
}

void MainWindow::onActionRestartTriggered()
{
    QList< int > ids = getSelectedItemIds();
    int restartCount = ids.size();
    if (restartCount == 0) {
        return;
    }

    QMessageBox::StandardButton ans;
    QString message = "Are you sure you want to restart this download?";
    if (restartCount > 1) {
        message = "You have selected " + QString::number(restartCount) + " downloads to restart\n";
        message.append("Are you sure you want to restart these downloads ?");
    }

    ans = QMessageBox::question(this, "Restart Confirmation?",
                                message, QMessageBox::Yes|QMessageBox::No);
    if (ans == QMessageBox::No) {
        return;
    }
    foreach(int id, ids) {
        restartDownload(id);
    }
}

void MainWindow::restartDownload(int id)
{
    auto it = m_downloads.find(id);
    if (it != m_downloads.end()) {
        Q_ASSERT(it.value() != nullptr);
        it.value()->stopDownload();
        it.value()->cleanUp();
        it.value()->deleteLater();
        m_downloads.remove(id);
    }

    int rowId = m_model->findRowByDatabaseId(id);
    m_model->setData(m_model->index(rowId, Enum::DownloadAttributes::Started), false);

    StartDownload *newDownload = new StartDownload(id);
    m_downloads.insert(id, newDownload);
    newDownload->startDownload();
}

void MainWindow::onActionStopTriggered()
{
    QList< int > ids = getSelectedItemIds();
    int stopCount = ids.size();
    if (stopCount == 0) {
        return;
    }

    int resumeNotSupportedCount = 0;
    foreach(int id, ids) {
        if (!checkResumeSupported(id)) {
            resumeNotSupportedCount += 1;
        }
    }

    if (resumeNotSupportedCount == 0) {
        foreach(int id, ids) {
            stopDownload(id);
        }
    } else {
        foreach(int id, ids) {
            if (checkResumeSupported(id)) {
                stopDownload(id);
            }
        }
        QMessageBox::StandardButton ans;
        QString message = "Some of the downloads you selected cannot be resumed. \n"
                          "Do you want to stop them ?";
        ans = QMessageBox::question(this, "Stop Confirmation?",
                                    message, QMessageBox::Yes|QMessageBox::No);
        if (ans == QMessageBox::Yes) {
            foreach(int id, ids) {
                if (checkResumeSupported(id)) {
                    stopDownload(id);
                }
            }
        }
    }
}

void MainWindow::stopDownload(qint64 id)
{
    QMap<qint64, StartDownload*>::iterator it = m_downloads.find(id);
    if (it != m_downloads.end()) {
        it.value()->stopDownload();
        it.value()->deleteLater();
        m_downloads.remove(id);
    }
}

void MainWindow::onActionRemoveTriggered()
{
    QList< int > ids = getSelectedItemIds();
    if (ids.length() == 0) {
        return;
    }

    QString message = "If you remove the download, download will be removed from list\n"
                      "Do you want to remove this download?";
    QMessageBox::StandardButton ans = QMessageBox::question(this, "Remove Confirmation",
                                      message, QMessageBox::Yes | QMessageBox::No);
    if (ans == QMessageBox::No) {
        return;
    }

    foreach(int id, ids) {
        stopDownload(id);
        m_model->removeDownloadFromModel(id);
    }
}

void MainWindow::showDownloadDialog(QString url)
{
    if (m_model->urlAlreadyInList(url)) {
        // ToDo: warn user that file is already present in the download list and ask for restart or resume if possible
    }

    QPointer<DownloadInfoDialog> infoDialog = QPointer<DownloadInfoDialog>(new DownloadInfoDialog());
    infoDialog->setWindowTitle("Add Download");
    infoDialog->setModal(true);
    infoDialog->ui->url->setText(url);
    FetchHeaders *fh = new FetchHeaders(url);
    connect(fh, &FetchHeaders::headersFetched, [=]() {
        if (!infoDialog.isNull()) {
            infoDialog->ui->size->setVisible(true);
            infoDialog->ui->size->setText(fh->size);
            infoDialog->ui->filePath->setEnabled(false);
            infoDialog->ui->filePath->setText(fh->fileName);
        }
    });

    connect(infoDialog->ui->startDownload, &QPushButton::clicked,[=]() {
        infoDialog->close();
        if (fh->headerFetchComplete) {
            qint64 id = m_model->insertDownloadIntoModel(&fh->properties);
            StartDownload *newDownload = new StartDownload(id);
            m_downloads.insert(id, newDownload);
            connect(newDownload, &StartDownload::downloadComplete, this, &MainWindow::afterDownloadCompleted);
            newDownload->startDownload();
        } else {
            connect(fh, &FetchHeaders::headersFetched, [=]() {
                qint64 id = m_model->insertDownloadIntoModel(&fh->properties);
                StartDownload *newDownload = new StartDownload(id);
                m_downloads.insert(id, newDownload);
                connect(newDownload, &StartDownload::downloadComplete, this, &MainWindow::afterDownloadCompleted);
                newDownload->startDownload();
            });
        }
    });
    infoDialog->exec();
    infoDialog->deleteLater();
}

void MainWindow::afterDownloadCompleted(int databaseId)
{
    QMap<qint64, StartDownload*>::iterator it = m_downloads.find(databaseId);
    if (it != m_downloads.end()) {
        it.value()->deleteLater();
        m_downloads.remove(databaseId);
    }
}

void MainWindow::saveHeaderState()
{
    QByteArray headerViewState = m_downloadView->horizontalHeader()->saveState();
    // ToDo: save this qbytearray in new table in database.
    // and add restoring function in load settings.
}

void MainWindow::about()
{
    qDebug() << "about";
    QLabel *icon = new QLabel;
    icon->setPixmap(QPixmap(":icons/download-2-icon.png"));

    QLabel *text = new QLabel;
    text->setWordWrap(true);
    text->setText("<p><b>SDM</b> is a download manager"
                  " which increases download speed and enables"
                  " user to download videos from various video"
                  " hosting websites like youtube, dailymotion etc."
                  " It is especially made for GNU/LINUX"
                  " but is also available for Windows.</p>"
                  "<p></p>");

    QPushButton *quitButton = new QPushButton("OK");

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->setMargin(10);
    topLayout->setSpacing(10);
    topLayout->addWidget(icon);
    topLayout->addWidget(text);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addStretch();
    bottomLayout->addWidget(quitButton);
    bottomLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(bottomLayout);

    QDialog about(this);
    about.setModal(true);
    about.setWindowTitle(tr("About SDM"));
    about.setLayout(mainLayout);

    connect(quitButton, SIGNAL(clicked()), &about, SLOT(close()));

    about.exec();
}

void MainWindow::saveSettings()
{
//     Prepare and reset the settings
//     QSettings settings("dm", "settings");
//     settings.clear();
//
//     settings.setValue("maxId", m_maxId);
//
//     settings.beginWriteArray("Downloads");
//     for (int i = 0; i < jobs.size(); ++i) {
//         settings.setArrayIndex(i);
//         settings.setValue("filename", jobs.at(i).filename);
//         settings.setValue("url", jobs.at(i).url);
//         settings.setValue("id", jobs.at(i).id);
//        settings.setValue("downloadedBytes", jobs.at(i)());
//        settings.setValue("size", jobs.at(i).client->downloadedBytes());
//        settings.setValue("resumeCapability", jobs.at(i).client->dumpedState());
//     }
//     settings.endArray();
//     settings.sync();
}

void MainWindow::loadSettings()
{
//     Prepare and reset the settings
//     QSettings settings("dm", "settings");
//     settings.clear();
//
//     settings.setValue("maxId", m_maxId);
//
//     settings.beginWriteArray("Downloads");
//     for (int i = 0; i < jobs.size(); ++i) {
//         settings.setArrayIndex(i);
//         settings.setValue("filename", jobs.at(i).filename);
//         settings.setValue("url", jobs.at(i).url);
//         settings.setValue("id", jobs.at(i).id);
//        settings.setValue("downloadedBytes", jobs.at(i)());
//        settings.setValue("size", jobs.at(i).client->downloadedBytes());
//        settings.setValue("resumeCapability", jobs.at(i).client->dumpedState());
//     }
//     settings.endArray();
//     settings.sync();
}
