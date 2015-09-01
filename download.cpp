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

#include "download.h"
#include "singletonfactory.h"

#include <curl/curl.h>

#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkReply>
#include <QDebug>
#include <cmath>
#include <QFile>
#include <QStandardPaths>
#include <QElapsedTimer>
#include <QTimer>
#include <QDataStream>

Download::Download(qint64 id, QString rawURL, qint64 start, qint64 end) : QObject(), downloadId(id)
{
    qnam = new QNetworkAccessManager();
    url = new QUrl(rawURL);

    rangeStart = start;
    rangeEnd = end;
    mMemoryDatabase = SingletonFactory::instanceFor<MemoryDatabase>();
    properties = DownloadProperties(mMemoryDatabase->getDetails(downloadId));

    tempFile = new QTemporaryFile("sdm");
    tempFile->setAutoRemove(false);
    if (!tempFile->open()) {
        qDebug() << "File Opening Failed";
        return;
    }
    fileName = tempFile->fileName();
    req = new QNetworkRequest();
    req->setUrl(*url);
    QString rangeString;
    if (end == -1) {
        rangeString = "bytes=" + QString::number(start) + "-";
    } else {
        rangeString = "bytes=" + QString::number(start) + "-" + QString::number(end);
    }
    req->setRawHeader("Range", rangeString.toLocal8Bit());

}

Download::~Download()
{
    qDebug() << "Download Destruction";
}

void Download::start()
{
    downloadReply = qnam->get(*req);

    connect(downloadReply, &QNetworkReply::downloadProgress, this, &Download::downloadProgress);
    connect(downloadReply, &QNetworkReply::finished, this, &Download::downloadFinished);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout ,this, &Download::updateDetails);
    timer->start(timeInterval);
    bytesProcessed = 0;
    bytesDownloaded = 0;
}

void Download::updateDetails()
{
    qint64 transferRate = (bytesDownloaded - bytesProcessed)/timeInterval;
    QHash<int, QVariant> details;
    details.insert(DownloadBackend::TransferRate, QVariant(transferRate));
    details.insert(DownloadBackend::BytesDownloaded, QVariant(bytesDownloaded - bytesProcessed));
    emit updateGui(details);
    bytesProcessed = bytesDownloaded;
}

void Download::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    tempFile->write(downloadReply->readAll());
    Q_ASSERT(tempFile->flush());
    bytesDownloaded = bytesReceived;
    update();
}

void Download::downloadFinished()
{
    qDebug() << "Download Finished";
    Q_ASSERT(tempFile->flush());
    update();
    emit downloadComplete();
}

void Download::errorOccured(QNetworkReply * reply, const QList<QSslError> & errors)
{
    qDebug() << "Error occured";
    reply->abort();
}

void Download::update()
{
    properties = DownloadProperties(mMemoryDatabase->getDetails(downloadId));
    QByteArray baOut = properties.tempFileNames;
    QDataStream dsOut(&baOut, QIODevice::ReadOnly);
    QMap <double, QMap <qint8, QVariant> > tempFilesMeta;
    dsOut >> tempFilesMeta;
    //operation on tempFilesMeta
    QMap<double, QMap <qint8, QVariant> >::iterator i;
    for (i = tempFilesMeta.begin(); i != tempFilesMeta.end(); ++i) {
        if (i.value().value(3).toString() != fileName) {
            continue;
        }
        qint64 start  = rangeStart;
        qint64 done = bytesDownloaded;
        qint64 end = rangeEnd;
        QString name = fileName;
        QMap <qint8, QVariant> newDownloadMeta;
        newDownloadMeta.insert(0, start);
        newDownloadMeta.insert(1, done);
        newDownloadMeta.insert(2, end);
        newDownloadMeta.insert(3, name);
        double key = i.key();
        tempFilesMeta.remove(key);
        tempFilesMeta.insert(key, newDownloadMeta);
        break;
    }
    QByteArray baIn = properties.tempFileNames;
    QDataStream dsIn(&baIn, QIODevice::WriteOnly);
    dsIn << tempFilesMeta;
    properties.tempFileNames = baIn;
    mMemoryDatabase->updateDetails(properties);
}

void Download::abortDownload()
{
    disconnect(downloadReply, &QNetworkReply::finished, this, &Download::downloadFinished);
    disconnect(downloadReply, &QNetworkReply::downloadProgress, this, &Download::downloadProgress);
    downloadReply->abort();
    tempFile->flush();
    this->deleteLater();
}
