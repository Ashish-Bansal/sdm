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
        menu->addAction(restoreAction);
        trayIcon->setVisible(true);
        trayIcon->show();
        connect(trayIcon, &QSystemTrayIcon::activated, this, &QWidget::showNormal);
        connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));
        trayIcon->showMessage("SDM", "SDM has been started", QSystemTrayIcon::Information, 1 * 1000);
    }
    m_model = SingletonFactory::instanceFor<DownloadModel>();

    QStringList headers = {"RowId", "DatabaseId", "Filename", "Size", "Progress", "Transfer Rate",
                           "Status", "Time Remaining", "Resume Capabilitiy", "Date"};

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
    connect(startDownload, &QAction::triggered, this, [=] {
//         auto items = m_downloadView->selectedItems();
//         foreach(auto item, items){
//             qint64 id = item->text(TableView::DatabaseId).toLongLong();
//             onActionResumeTriggered(id);
//         }
    });
    connect(restartDownload, &QAction::triggered, this, [=] {
//         auto items = m_downloadView->selectedItems();
//         foreach(auto item, items){
//             qint64 id = item->text(TableView::DatabaseId).toLongLong();
//             onActionRestartTriggered(id);
//         }
    });
    connect(stopDownload, &QAction::triggered, this, [=] {
//         auto items = m_downloadView->selectedItems();
//         foreach(auto item, items){
//             qint64 id = item->text(TableView::DatabaseId).toLongLong();
//             onActionStopTriggered(id);
//         }
    });
    connect(removeDownload, &QAction::triggered, this, [=] {
//         auto items = m_downloadView->selectedItems();
//         foreach(auto item, items){
//             qint64 id = item->text(TableView::DatabaseId).toLongLong();
//             onActionRemoveTriggered(id);
//         }
    });
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

void MainWindow::onActionAddTriggered()
{
    AddDialog addDialog;
    addDialog.setWindowTitle("Add Download");
    addDialog.setModal(true);
    connect(&addDialog, &AddDialog::showDownloadDialog, this, &MainWindow::showDownloadDialog);
    addDialog.exec();
}

void MainWindow::onActionResumeTriggered(qint64 id)
{
    if (checkResumeSupported(id)) {
        StartDownload *newDownload = new StartDownload(id);
        m_downloads.insert(id, newDownload);
        newDownload->startDownload();
    } else {
        QString message = "This download cannot be resumed\n"
                          "Would you like to restart this download?";
        onActionRestartTriggered(id, message);
    }
}

bool MainWindow::checkResumeSupported(qint64 id)
{
    const DownloadAttributes *properties = m_model->getDetails(id);
    if (properties->resumeCapability == Enum::SDM::ResumeSupported) {
        return true;
    }
    return false;
}

void MainWindow::onActionRestartTriggered(qint64 id, QString message)
{
    QMessageBox::StandardButton ans;
    message = message.isEmpty() ? "Are you sure you want to restart this download?" : message;
    ans = QMessageBox::question(this, "Restart Confirmation?",
                                message, QMessageBox::Yes|QMessageBox::No);
    if (ans == QMessageBox::No) {
        return;
    }

    auto it = m_downloads.find(id);
    if (it != m_downloads.end()) {
        it.value()->stopDownload();
        it.value()->deleteLater();
        m_downloads.remove(id);
    }

//     if (m_model->restartDownload(id) == Enum::SDM::Failed) {
//         QMessageBox::critical(this, "Failed", "Unable to Restart Download");
//         return;
//     }

    StartDownload *newDownload = new StartDownload(id);
    m_downloads.insert(id, newDownload);
    newDownload->startDownload();
}

void MainWindow::onActionStopTriggered(qint64 id)
{
    if (checkResumeSupported(id)) {
        stopDownload(id);
        return;
    }

    QString message = "This download cannot be resumed\n"
                      "Would you like to stop this download?";
    QMessageBox::StandardButton ans =
            QMessageBox::question(this, "Stop Confirmation",
                                  message, QMessageBox::Yes | QMessageBox::No);
    if (ans == QMessageBox::Yes) {
        stopDownload(id);
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
//     clearTreeItem(id);
}
/*
void MainWindow::clearTreeItem(qint64 id)
{
    QTreeWidgetItem *item = getTreeItem(id);
    if (item == nullptr) {
        return;
    }
    item->setText(Enum::TableView::TransferRate, "");
    item->setText(Enum::TableView::Status, "Idle");
    item->setText(Enum::TableView::TimeRemaining, "");
}*/

void MainWindow::onActionRemoveTriggered(qint64 id)
{
    QString message = "If you remove the download, it would be lost permanently\n"
                      "Do you want to remove this download?";
    QMessageBox::StandardButton ans =
            QMessageBox::question(this, "Remove Confirmation",
                                  message, QMessageBox::Yes | QMessageBox::No);
    if (ans == QMessageBox::No) {
        return;
    }

    stopDownload(id);
    m_model->removeDownloadFromModel(id);
}

void MainWindow::fileAlreadyInList(DownloadAttributes properties)
{
    //warn user that file is already present in the download list and ask for restart or resume if possible
}

void MainWindow::showDownloadDialog(QString url)
{
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

    connect(infoDialog->ui->startDownload, &QPushButton::clicked,[=](){
        infoDialog->close();
        if (fh->headerFetchComplete) {
            qint64 id = m_model->insertDownloadIntoModel(&fh->properties);
            StartDownload *newDownload = new StartDownload(id);
            m_downloads.insert(id, newDownload);
            newDownload->startDownload();
        } else {
            connect(fh, &FetchHeaders::headersFetched, [=](){
                qint64 id = m_model->insertDownloadIntoModel(&fh->properties);
                StartDownload *newDownload = new StartDownload(id);
                m_downloads.insert(id, newDownload);
                newDownload->startDownload();
            });
        }
    });
    infoDialog->exec();
    infoDialog->deleteLater();
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
