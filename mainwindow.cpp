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
#include "adddialog.h"
#include "downloadinfodialog.h"
#include "downloadprocessor.h"
#include "singletonfactory.h"
#include "updater.h"
#include "debug.h"
#include "global.h"

#include <QtWidgets>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qInstallMessageHandler(Debug::debugStyle);
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

    mProxyModel = SingletonFactory::instanceFor<ProxyModel>();
    mDownloadModel = SingletonFactory::instanceFor<DownloadModel>();
    mProxyModel->setSourceModel(mDownloadModel);

    mDownloadView = new DownloadView(this);
    mDownloadView->setModel(mProxyModel);
    mDownloadView->loadViewSettings();

    connect(mDownloadView->horizontalHeader(), &QHeaderView::sectionPressed, this, &MainWindow::saveHeaderState);
    QVBoxLayout *layout = new QVBoxLayout();
    QLineEdit *ledit = new QLineEdit();
    ledit->setPlaceholderText("Search Box");
    layout->addWidget(ledit);
    layout->addWidget(mDownloadView);
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

    connect(&mWebSocketServer, &WebSocketServer::downloadRequested, this, &MainWindow::showDownloadDialog);

    connect(ledit, &QLineEdit::textChanged, this, &MainWindow::onFilterTextChanged);
    ledit->setClearButtonEnabled(true);

    QMetaObject::invokeMethod(this, "loadSettings", Qt::QueuedConnection);
    mWebSocketServer.startListening();
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
    mProxyModel->readDatabase();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
    hide();
}

QList< int > MainWindow::getSelectedItemIds()
{
    QList< int > ids;
    QItemSelectionModel *selectedItemModel = mDownloadView->selectionModel();
    auto items = selectedItemModel->selectedRows(Enum::DownloadAttributes::DatabaseId);
    foreach(auto item, items) {
        qint64 id = mProxyModel->data(item, Qt::DisplayRole).toInt();
        ids.append(id);
    }
    return ids;
}

void MainWindow::onActionAddTriggered()
{
    AddDialog addDialog;
    addDialog.setWindowTitle("Add Download");
    addDialog.setModal(true);
    connect(&addDialog, &AddDialog::showDownloadDialog, [=](QString url) {
        showDownloadDialog(url, HeaderList());
    });
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
            DownloadProcessor *downloadProcessor = new DownloadProcessor(id);
            mDownloadProcessorMap.insert(id, downloadProcessor);
            downloadProcessor->startDownload();
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
    auto it = mDownloadProcessorMap.find(id);
    if (it != mDownloadProcessorMap.end()) {
        it.value()->startDownload();
    }
}

bool MainWindow::checkResumeSupported(int id)
{
    const DownloadAttributes *properties = mProxyModel->getDetails(id);
    if (properties->getValue(Enum::DownloadAttributes::ResumeCapability) == Enum::SDM::ResumeSupported) {
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
    auto it = mDownloadProcessorMap.find(id);
    if (it != mDownloadProcessorMap.end()) {
        Q_ASSERT(it.value() != nullptr);
        it.value()->stopDownload();
        it.value()->cleanUp();
        it.value()->deleteLater();
        mDownloadProcessorMap.remove(id);
    }

    int rowId = mProxyModel->findRowByDatabaseId(id);
    mProxyModel->setData(mProxyModel->index(rowId, Enum::DownloadAttributes::Started), false);

    DownloadProcessor *downloadProcessor = new DownloadProcessor(id);
    mDownloadProcessorMap.insert(id, downloadProcessor);
    downloadProcessor->startDownload();
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
    QMap<qint64, DownloadProcessor*>::iterator it = mDownloadProcessorMap.find(id);
    if (it != mDownloadProcessorMap.end()) {
        it.value()->stopDownload();
        it.value()->deleteLater();
        mDownloadProcessorMap.remove(id);
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
        mProxyModel->removeDownloadFromModel(id);
    }
}

void MainWindow::showDownloadDialog(QString url, HeaderList headers)
{
    if (mProxyModel->urlAlreadyInList(url)) {
        // ToDo: warn user that file is already present in the download list and ask for restart or resume if possible
    }

    QPointer<DownloadInfoDialog> infoDialog = QPointer<DownloadInfoDialog>(new DownloadInfoDialog());
    infoDialog->setWindowTitle("Add Download");
    infoDialog->setModal(false);
    infoDialog->setUrl(url);
    infoDialog->setWindowFlags(Qt::SubWindow);
    FetchHeaders *fh = new FetchHeaders(url, headers);
    fh->fetchHeaders();

    connect(fh, &FetchHeaders::headersFetched, [=]() {
        if (!infoDialog.isNull()) {
            int filesize = fh->filesize();
            QString filesizeString = SDM::convertUnits(filesize);
            infoDialog->setSize(filesizeString);
            infoDialog->setPath(fh->filename());
        }
    });

    connect(infoDialog, &DownloadInfoDialog::downloadNow,[=]() {
        infoDialog->close();
        int databaseId = mProxyModel->insertDownloadIntoModel(fh->properties());
        if (fh->fetchHeadersCompleted()) {
            mProxyModel->updateDetails(databaseId, fh->properties());
            DownloadProcessor *downloadProcessor = new DownloadProcessor(databaseId);
            mDownloadProcessorMap.insert(databaseId, downloadProcessor);
            connect(downloadProcessor, &DownloadProcessor::downloadComplete, this, &MainWindow::afterDownloadCompleted);
            downloadProcessor->startDownload();
        } else {
            connect(fh, &FetchHeaders::headersFetched, [=]() {
                mProxyModel->updateDetails(databaseId, fh->properties());
                DownloadProcessor *downloadProcessor = new DownloadProcessor(databaseId);
                mDownloadProcessorMap.insert(databaseId, downloadProcessor);
                connect(downloadProcessor, &DownloadProcessor::downloadComplete, this, &MainWindow::afterDownloadCompleted);
                downloadProcessor->startDownload();
            });
        }
        infoDialog->deleteLater();
    });
    infoDialog->show();
}

void MainWindow::afterDownloadCompleted(int databaseId)
{
    QMap<qint64, DownloadProcessor*>::iterator it = mDownloadProcessorMap.find(databaseId);
    if (it != mDownloadProcessorMap.end()) {
        it.value()->deleteLater();
        mDownloadProcessorMap.remove(databaseId);
    }
}

void MainWindow::onFilterTextChanged(QString text)
{
    QRegExp regExp(text, Qt::CaseInsensitive);
    mProxyModel->setFilterRegExp(regExp);
}

void MainWindow::saveHeaderState()
{
    QByteArray headerViewState = mDownloadView->horizontalHeader()->saveState();
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
}

void MainWindow::loadSettings()
{
}
