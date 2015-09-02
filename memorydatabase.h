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

#ifndef MEMORYDATABASE_H
#define MEMORYDATABASE_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QSharedPointer>

#include "databasemanager.h"
#include "downloadattributes.h"
#include "downloadmodel.h"

class MemoryDatabase : public QObject
{
    Q_OBJECT
public:
    MemoryDatabase();
    ~MemoryDatabase();
    qint64 insertDownload(DownloadAttributes properties);
    void removeDownload(qint64 id);
    void updateDetails(DownloadAttributes properties);
    const DownloadAttributes* getDetails(qint64 id);
    qint64 maxId();
    void readDatabase();
    int restartDownload(qint64 id);
    void setModel(QSharedPointer<DownloadModel> model);

private:
    QMap<int, DownloadAttributes*> m_downloadList;
    DatabaseManager *m_dbManager;
    QSharedPointer<DownloadModel> m_model;

public slots:
    void writeToDatabase();

signals:
    void downloadRemoved(qint64 id);
    void downloadInserted(qint64 id);
    void downloadFinished(qint64 id);
    void writeDatabase();
    void updateGUI(qint64 id);
    void downloadLoaded(qint64 id);
};

#endif // MEMORYDATABASE_H
