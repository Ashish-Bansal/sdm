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


#include "enum.h"
#include "startdownload.h"
#include "singletonfactory.h"
#include "downloadproperties.h"

#include <algorithm>
#include <QDir>

StartDownload::StartDownload(int id) : id(id)
{
    qDebug() << id;
    mMemoryDatabase = SingletonFactory::instanceFor<MemoryDatabase>();
    properties = DownloadProperties(mMemoryDatabase->getDetails(id));
    filename = properties.filename;
    resumeSupported = properties.resumeCapability;
    filesize = properties.filesize;
    url = properties.url;
    isAlreadyStarted = properties.started;
    counter = 0;
    totalBytesDownloaded = 0;

    if (isAlreadyStarted) {
        QByteArray baOut = properties.tempFileNames;
        QDataStream dsOut(&baOut, QIODevice::ReadOnly);
        QMultiMap <double, QMultiMap <qint8, QVariant> > savedFilesMeta;
        QMultiMap <double, QMultiMap <qint8, QVariant> > newFilesMeta;
        dsOut >> savedFilesMeta;

        //operation on savedFilesMeta
        QMultiMap <double, QMultiMap <qint8, QVariant>>::iterator i;
        for (i = savedFilesMeta.begin(); i != savedFilesMeta.end(); ++i) {
            qint64 start  = i.value().value(0).toLongLong();
            qint64 done = i.value().value(1).toLongLong();
            qint64 end = i.value().value(2).toLongLong();
            if(start + done >= end) continue;
            Download *download = new Download(id, url, start + done, end);
            dwldip.append(download);

            QMultiMap <qint8, QVariant> newDownloadMeta;
            newDownloadMeta.insert(0, start + done);
            newDownloadMeta.insert(1, 0);
            newDownloadMeta.insert(2, end);
            newDownloadMeta.insert(3, download->fileName);

            QMultiMap <qint8, QVariant> changedDownloadMeta;
            changedDownloadMeta.insert(0, start);
            changedDownloadMeta.insert(1, done);
            changedDownloadMeta.insert(2, start+done);
            changedDownloadMeta.insert(3, i.value().value(3).toString());

            double x, y;
            x = i.key();
            i++;
            if (i == savedFilesMeta.end()) {
                y = x + 1;
            } else {
                y = (x + i.key())/2;
            }
            i--;
            savedFilesMeta.replace(i.key(), changedDownloadMeta);
            newFilesMeta.insert(x + y, newDownloadMeta);

            connect(download, &Download::downloadComplete, this, &StartDownload::writeToFileInParts);
            connect(download, &Download::updateGui, this, &StartDownload::updateDatabase);
        }
        QByteArray baIn;
        QDataStream dsIn(&baIn, QIODevice::WriteOnly);
        dsIn << savedFilesMeta + newFilesMeta;
        properties.tempFileNames = baIn;
        mMemoryDatabase->updateDetails(properties);
    } else {
        if (resumeSupported) {
            qDebug() << "Downloading In parts supported";
            dwldip.append(new Download(id, url, 0, filesize/6));
            dwldip.append(new Download(id, url, filesize/6 + 1, 2*filesize/6));
            dwldip.append(new Download(id, url, 2*filesize/6 + 1, 3*filesize/6));
            dwldip.append(new Download(id, url, 3*filesize/6 + 1, 4*filesize/6));
            dwldip.append(new Download(id, url, 4*filesize/6 + 1, 5*filesize/6));
            dwldip.append(new Download(id, url, 5*filesize/6 + 1, filesize));
            QList <Download*>::iterator i;
            QMultiMap <double, QMultiMap <qint8, QVariant> > tempFilesMeta;
            int counter = 0;
            for (i = dwldip.begin(); i != dwldip.end(); i++, counter++) {
                connect(*i, &Download::downloadComplete, this, &StartDownload::writeToFileInParts);
                connect(*i, &Download::updateGui, this, &StartDownload::updateDatabase);
                QMultiMap <qint8, QVariant> newDownloadMeta;
                newDownloadMeta.insert(0, (*i)->rangeStart);
                newDownloadMeta.insert(1, 0);
                newDownloadMeta.insert(2, (*i)->rangeEnd);
                newDownloadMeta.insert(3, (*i)->fileName);
                tempFilesMeta.insert(counter, newDownloadMeta);
            }

            QByteArray baIn;
            QDataStream dsIn(&baIn, QIODevice::WriteOnly);
            dsIn << tempFilesMeta;
            properties.tempFileNames = baIn;
            mMemoryDatabase->updateDetails(properties);
        } else {
            qDebug() << "Downloading In parts *NOT* supported";
            dwldaw = new Download(id, url,0, -1);
            connect(dwldaw, &Download::downloadComplete, this, &StartDownload::writeToFileAsWhole);
            connect(dwldaw, &Download::updateGui, this, &StartDownload::updateDatabase);
        }
        properties.started = true;
        mMemoryDatabase->updateDetails(properties);
    }
}

void StartDownload::updateDatabase(QHash<int, QVariant> details)
{
    fetchProperties();
    totalBytesDownloaded += details.value(DownloadBackend::BytesDownloaded).toLongLong();
//    qDebug() << totalBytesDownloaded;
//    qDebug() << properties;
    properties.transferRate = details.value(DownloadBackend::TransferRate).toString();
    properties.bytesDownloaded = totalBytesDownloaded;
    mMemoryDatabase->updateDetails(properties);
//    qDebug() << properties;
}

bool StartDownload::compareList(QPair<double, QPair<qint64, QString>> i, QPair<double, QPair<qint64, QString>> j)
{
    return i.first < j.first;
}

void StartDownload::writeToFileInParts()
{
    fetchProperties();
    QByteArray baOut = properties.tempFileNames;
    QDataStream dsOut(&baOut, QIODevice::ReadOnly);
    QMultiMap <double, QMultiMap <qint8, QVariant> > tempFilesMeta;
    dsOut >> tempFilesMeta;
    qDebug() << tempFilesMeta;
    QMultiMap<double, QMultiMap <qint8, QVariant> >::iterator i;
    if (i == tempFilesMeta.end()) {
        return;
    }
    for (i = tempFilesMeta.begin(); i != tempFilesMeta.end(); ++i) {
        if (i.value().value(0).toLongLong() + i.value().value(1).toLongLong() < i.value().value(2).toLongLong()) {
            return;
        }
    }

    QList <Download*>::iterator it;
    for (it = dwldip.begin(); it != dwldip.end(); it++) {
        (*it)->tempFile->close();
        (*it)->disconnect();
        (*it)->deleteLater();
    }

    properties.status = "Merging";
    mMemoryDatabase->updateDetails(properties);
    qDebug() << QDir::homePath() + "/sdm/" + filename;
    file.setFileName(QDir::homePath() + "/sdm/" + filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Unable to open file, So opening randomfile";
        file.setFileName(QDir::homePath() + "/sdm/"  + "filename" + QString::number(qrand()));
        file.open(QIODevice::WriteOnly);
    }

    QList< QPair< double, QPair<qint64, QString> > > p;
    for (i = tempFilesMeta.begin(); i != tempFilesMeta.end(); ++i) {
        QString filename = i.value().value(3).toString();
        double d = i.key();
        p << QPair< double, QPair<qint64, QString> > (d, QPair<qint64, QString>(i.value().value(1).toLongLong(), filename));
    }
    std::sort(p.begin(), p.end(), compareList);
    QList< QPair< double, QPair<qint64, QString> > >::iterator listIt;
    for (listIt = p.begin(); listIt != p.end(); listIt++) {
        QString filename = (*listIt).second.second;
        QFile tempFile(filename);
        if (!tempFile.open(QIODevice::ReadOnly)) {
            qDebug() << "Error while opening file :" << filename;
            return;
        }
        tempFile.seek(0);
        file.write(tempFile.read((*listIt).second.first));
//        tempFile.remove();
    }
    file.close();
    dwldip.clear();
    cleanUp();
//     emit downloadComplete(id);
    qDebug() << "Write Complete In Parts";

    properties.status = "Completed";
    mMemoryDatabase->updateDetails(properties);
    baOut = properties.tempFileNames;
    QDataStream sOut(&baOut, QIODevice::ReadOnly);
    QMultiMap <double, QMultiMap <qint8, QVariant> > savedFilesMeta;
    sOut >> savedFilesMeta;
    qDebug() << savedFilesMeta;

    this->deleteLater();
}

void StartDownload::writeToFileAsWhole()
{
    fetchProperties();
    properties.status = "Merging";
    mMemoryDatabase->updateDetails(properties);
    qDebug() << QDir::homePath() + "/sdm/" + filename;
    file.setFileName(QDir::homePath() + "/sdm/" + filename);
    if(!file.open(QIODevice::WriteOnly)){
        qDebug() << "Unable to open file, So opening randomfile";
        file.setFileName(QDir::homePath() + "/sdm/" + "filename" + QString::number(qrand()));
        file.open(QIODevice::WriteOnly);
    }
    dwldaw->tempFile->seek(0);
    file.write(dwldaw->tempFile->readAll());
    file.close();
    dwldaw->tempFile->remove();
    cleanUp();
//     emit downloadComplete(id);
    qDebug() << "Write Complete As Whole";
    properties.status = "Completed";
    properties.transferRate = "";
    mMemoryDatabase->updateDetails(properties);
    this->deleteLater();
    dwldaw->disconnect();
}

void StartDownload::cleanUp()
{
    if (resumeSupported == true) {
        QList <Download*>::iterator i;
        for (i = dwldip.begin(); i != dwldip.end(); i++) {
            (*i)->tempFile->remove();
            (*i)->deleteLater();
        }
    } else {
        dwldaw->tempFile->remove();
        dwldaw->deleteLater();
    }
}

void StartDownload::fetchProperties()
{
    properties = DownloadProperties(mMemoryDatabase->getDetails(id));
}

void StartDownload::stopDownload()
{
    if (resumeSupported) {
        QList<Download*>::Iterator it;
        for(it = dwldip.begin(); it != dwldip.end(); it++) {
            (*it)->abortDownload();
        }
    } else {
        dwldaw->abortDownload();
    }
    this->deleteLater();
}
