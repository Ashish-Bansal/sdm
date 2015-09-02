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
