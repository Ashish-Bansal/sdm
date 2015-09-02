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

#ifndef STARTDOWNLOAD_H
#define STARTDOWNLOAD_H

#include "download.h"
#include "memorydatabase.h"

#include <QObject>
#include <QFile>
#include <QHash>
#include <QVariant>

class StartDownload : public QObject
{
    Q_OBJECT
public:
    StartDownload(int id);
    void cleanUp();
    void updateDatabase(QHash<int, QVariant> details);
    void startDownload();
    void stopDownload();

    QList <Download*> dwldip;
    Download *dwldaw;
    QFile file;

    bool resumeSupported = false;
    int counter;
    qint64 totalBytesDownloaded;
    qint64 id;
    QString url;
    qint64 filesize;
    QString filename;
    bool isAlreadyStarted;
    DownloadAttributes properties;

private:
    MemoryDatabase *m_memoryDatabase;
    void fetchProperties();
    static bool compareList(QPair<double, QPair<qint64, QString>> i, QPair<double, QPair<qint64, QString>> j);

private slots:
    void writeToFileInParts();
    void writeToFileAsWhole();
};

#endif // STARTDOWNLOAD_H
