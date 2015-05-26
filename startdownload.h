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

    QList <Download *> dwldip;
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
    DownloadProperties properties;

private:
    MemoryDatabase *mMemoryDatabase;
    void fetchProperties();
    static bool compareList(QPair<double, QPair<qint64, QString>> i, QPair<double, QPair<qint64, QString>> j);


private slots:
    void writeToFileInParts();
    void writeToFileAsWhole();
};

#endif // STARTDOWNLOAD_H
