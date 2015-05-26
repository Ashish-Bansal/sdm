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

#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include "enum.h"
#include "memorydatabase.h"
#include "downloadproperties.h"

#include <QObject>
#include <QNetworkReply>
#include <QString>
#include <QFile>
#include <QElapsedTimer>
#include <QTemporaryFile>
class Download : public QObject
{
    Q_OBJECT
public:
    Download(qint64 id, QString rawURL, qint64 start, qint64 end);
    ~Download();

    QString fileName;
    qint64 rangeStart;
    qint64 rangeEnd;
    qint64 downloadId;
    QTemporaryFile *tempFile;
    qint64 bytesDownloaded;
    qint64 bytesProcessed;
    qint64 timeInterval = 100;
    void abortDownload();

signals:
    void downloadComplete();
    void updateGui(QHash<int, QVariant> details);

private:
    QNetworkAccessManager *qnam;
    QNetworkRequest *req;
    QNetworkReply *downloadReply;
    MemoryDatabase *mMemoryDatabase;
    DownloadProperties properties;

    QUrl *url;
    int requestedContentLength;
    int responseContentLength;
    int originalContentLength;
    QElapsedTimer downloadTimer;
    QElapsedTimer timeGap;
    qint64 previousDownloadSize;
    void update();
    void updateDetails();

private slots:
    void downloadFinished(QNetworkReply *pReply);
    void errorOccured(QNetworkReply *reply, const QList<QSslError> &errors);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

};

#endif // DOWNLOAD_H
