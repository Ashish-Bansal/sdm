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
#include "downloadattributes.h"

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
    QTemporaryFile *tempFile;
    qint64 rangeStart;
    qint64 rangeEnd;
    qint64 downloadId;
    qint64 bytesDownloaded;
    qint64 bytesProcessed;
    void abortDownload();
    void start();

signals:
    void downloadComplete();
    void updateGui(QHash<int, QVariant> details);

private:
    QNetworkAccessManager *m_qnam;
    QNetworkRequest *m_req;
    QNetworkReply *m_downloadReply;
    MemoryDatabase *m_memoryDatabase;
    DownloadAttributes m_properties;
    QUrl *m_url;
    void update();
    void updateDetails();
    qint64 m_timeInterval;

private slots:
    void downloadFinished();
    void errorOccured(QNetworkReply *reply, const QList<QSslError> &errors);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

};

#endif // DOWNLOAD_H
