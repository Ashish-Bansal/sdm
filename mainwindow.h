/***********************************************************************************
 *   Copyright 2015 Ashish Bansal<bansal.ashish096@gmail.com>                      *
 *                                                                                 *
 *                                                                                 *
 *   This library is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public                    *
 *   License as published by the Free Software Foundation; either                  *
 *   version 2.1 of the License, or (at your option) any later version.            *
 *                                                                                 *
 *   This library is distributed in the hope that it will be useful,               *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of                *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
 *   Lesser General Public License for more details.                               *
 *                                                                                 *
 *   You should have received a copy of the GNU Lesser General Public              *
 *   License along with this library.  If not, see <http://www.gnu.org/licenses/>. *
 ***********************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "startdownload.h"
#include "memorydatabase.h"
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
    MemoryDatabase *m_memoryDatabase;
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
    void updateDetails(qint64 id);
    void stopDownload(qint64 id);
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
