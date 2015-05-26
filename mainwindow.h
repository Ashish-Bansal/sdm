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

#include <QMainWindow>
#include <QLabel>
#include <QSlider>
#include <QPointer>
#include <QTreeWidgetItem>
#include <QThread>

class DownloadView;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QAction *upActionTool;
    QAction *downActionTool;
    bool saveChanges;
    void saveSettings();
    void loadSettings();
    struct TableItem{
        qint64 id;
        qint64 rowId;
    };
    qint64 maxId = -1;
    QList<TableItem> table;
    QPair<double, QString> convertUnits(qint64 bytes);
    void downloadAdded(qint64 id);
    QTreeWidgetItem* getTreeItem(int id);
    void loadDownloads();
    bool checkResumeSupported(qint64 id);
    void fileAlreadyInList(DownloadProperties properties);

private:
    QPointer<DownloadView> downloadView;
    MemoryDatabase *mMemoryDatabase;
    QMap<qint64, StartDownload*> downloads;
    QThread workerThread;
    QThread downloadThread;

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
