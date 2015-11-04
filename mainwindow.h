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

#include "startdownload.h"
#include "downloadmodel.h"
#include "downloadmodel.h"
#include "downloadview.h"

#include <QMainWindow>
#include <QPointer>
#include <QTreeWidgetItem>
#include <QThread>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void saveSettings();
    void loadSettings();
    QPair<double, QString> convertUnits(qint64 bytes);
    void downloadAdded(qint64 id);
    QTreeWidgetItem* getTreeItem(int id);
    void loadDownloads();
    bool checkResumeSupported(qint64 id);
    void fileAlreadyInList(DownloadAttributes properties);

private:
    qint64 m_maxId;
    DownloadModel *m_model;
    QPointer<DownloadView> m_downloadView;
    QMap<qint64, StartDownload*> m_downloads;
    QThread m_workerThread;
    QThread m_downloadThread;
    void clearTreeItem(qint64 id);

private slots:
    void exit();
    void about();
    void onActionAddTriggered();
    void onActionResumeTriggered(qint64 id);
    void onActionRestartTriggered(qint64 id, QString message = QString());
    void onActionStopTriggered(qint64 id);
    void onActionRemoveTriggered(qint64 id);
    void showDownloadDialog(QString url);
    void stopDownload(qint64 id);
    void closeEvent(QCloseEvent *event);
    void saveHeaderState();
};

#endif // MAINWINDOW_H
