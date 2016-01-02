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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "downloadprocessor.h"
#include "downloadmodel.h"
#include "proxymodel.h"
#include "downloadview.h"
#include "websocketserver.h"
#include "updater.h"

#include <QMainWindow>
#include <QPointer>
#include <QList>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void saveSettings();
    void loadSettings();
    void loadDownloads();
    bool checkResumeSupported(int id);

private:
    ProxyModel *mProxyModel;
    DownloadModel *mDownloadModel;
    QPointer<DownloadView> mDownloadView;
    QMap<qint64, DownloadProcessor*> mDownloadProcessorMap;
    WebSocketServer mWebSocketServer;
    Updater updater;

private slots:
    void exit();
    void about();
    void onActionAddTriggered();
    void onActionResumeTriggered();
    void onActionRestartTriggered();
    void onActionStopTriggered();
    void onActionRemoveTriggered();
    void showDownloadDialog(QString url);
    void stopDownload(qint64 id);
    void closeEvent(QCloseEvent *event);
    void saveHeaderState();
    void restartDownload(int id);
    void resumeDownload(int id);
    void afterDownloadCompleted(int databaseId);
    void onFilterTextChanged(QString text);
    QList< int > getSelectedItemIds();
};

#endif // MAINWINDOW_H
