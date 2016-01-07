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
#include "global.h"
#include "downloadprocessor.h"
#include "singletonfactory.h"
#include "downloadattributes.h"

#include <QDir>

DownloadProcessor::DownloadProcessor(int id) : id(id)
{
    mDownloadModel = SingletonFactory::instanceFor< DownloadModel >();
    const DownloadAttributes *prop = mDownloadModel->getDetails(id);
    Q_ASSERT(prop != nullptr);
    properties = DownloadAttributes(prop);
    filename = properties.getValue(Enum::DownloadAttributes::Filename).toString();
    resumeSupported = properties.getValue(Enum::DownloadAttributes::ResumeCapability).toBool();
    filesize = properties.getValue(Enum::DownloadAttributes::Filesize).toInt();
    url = properties.getValue(Enum::DownloadAttributes::Url).toString();
    isAlreadyStarted = properties.getValue(Enum::DownloadAttributes::Started).toBool();
    counter = 0;
    totalBytesDownloaded = 0;
}

void DownloadProcessor::startDownload()
{
    if (isAlreadyStarted) {
        QByteArray b = properties.getValue(Enum::DownloadAttributes::TempFileNames).toByteArray();
        auto savedFilesMeta = SDM::readByteArray(b);
        auto newFilesMeta =  savedFilesMeta;
        newFilesMeta.clear();
        //operation on savedFilesMeta
        auto i = savedFilesMeta.begin();
        for (; i != savedFilesMeta.end(); ++i) {
            qint64 start  = i.value().value(0).toLongLong();
            qint64 done = i.value().value(1).toLongLong();
            qint64 end = i.value().value(2).toLongLong();
            if (properties.getValue(Enum::DownloadAttributes::Filesize) == end) {
                if((start + done) == end) continue;
            } else {
                if((start + done) > end) continue;
            }
            Download *download = new Download(id, url, start + done, end);
            dwldip.append(download);

            QMultiMap<qint8, QVariant> newDownloadMeta;
            newDownloadMeta.insert(0, start + done);
            newDownloadMeta.insert(1, 0);
            newDownloadMeta.insert(2, end);
            newDownloadMeta.insert(3, download->fileName);

            QMultiMap<qint8, QVariant> changedDownloadMeta;
            changedDownloadMeta.insert(0, start);
            changedDownloadMeta.insert(1, done);
            changedDownloadMeta.insert(2, start+done-1);
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
            newFilesMeta.insert(y, newDownloadMeta);

            connect(download, &Download::downloadComplete, this, &DownloadProcessor::writeToFileInParts);
            connect(download, &Download::updateGui, this, &DownloadProcessor::updateDatabase);
        }
        b = SDM::writeToByteArray(savedFilesMeta + newFilesMeta);
        properties.setValue(Enum::DownloadAttributes::TempFileNames, b);
        mDownloadModel->updateDetails(properties);
        for (auto g = dwldip.begin(); g != dwldip.end(); g++) {
            (*g)->start();
        }
    } else {
        if (resumeSupported) {
            qDebug() << "Downloading In parts supported";
            dwldip.append(new Download(id, url, 0, filesize/6));
            dwldip.append(new Download(id, url, filesize/6 + 1, 2*filesize/6));
            dwldip.append(new Download(id, url, 2*filesize/6 + 1, 3*filesize/6));
            dwldip.append(new Download(id, url, 3*filesize/6 + 1, 4*filesize/6));
            dwldip.append(new Download(id, url, 4*filesize/6 + 1, 5*filesize/6));
            dwldip.append(new Download(id, url, 5*filesize/6 + 1, filesize));
            QList<Download*>::iterator i;
            QMultiMap<double, QMultiMap<qint8, QVariant>> tempFilesMeta;
            int counter = 0;
            for (i = dwldip.begin(); i != dwldip.end(); i++, counter++) {
                connect(*i, &Download::downloadComplete, this, &DownloadProcessor::writeToFileInParts);
                connect(*i, &Download::updateGui, this, &DownloadProcessor::updateDatabase);
                QMultiMap<qint8, QVariant> newDownloadMeta;
                newDownloadMeta.insert(0, (*i)->rangeStart);
                newDownloadMeta.insert(1, 0);
                newDownloadMeta.insert(2, (*i)->rangeEnd);
                newDownloadMeta.insert(3, (*i)->fileName);
                tempFilesMeta.insert(counter, newDownloadMeta);
            }

            QByteArray b = SDM::writeToByteArray(tempFilesMeta);
            properties.setValue(Enum::DownloadAttributes::TempFileNames, b);
            mDownloadModel->updateDetails(properties);
            for (i = dwldip.begin(); i != dwldip.end(); i++) {
                (*i)->start();
            }
        } else {
            qDebug() << "Downloading In parts *NOT* supported";
            dwldaw = new Download(id, url, 0, -1);
            connect(dwldaw, &Download::downloadComplete, this, &DownloadProcessor::writeToFileAsWhole);
            connect(dwldaw, &Download::updateGui, this, &DownloadProcessor::updateDatabase);
            dwldaw->start();
        }
        properties.setValue(Enum::DownloadAttributes::Started, true);
        mDownloadModel->updateDetails(properties);
    }
    properties.setValue(Enum::DownloadAttributes::Status, Enum::Status::Downloading);
    mDownloadModel->updateDetails(properties);
}

void DownloadProcessor::updateDatabase(QHash<int, QVariant> details)
{
    fetchProperties();
    totalBytesDownloaded += details.value(Enum::DownloadBackend::BytesDownloaded).toLongLong();
    properties.setValue(Enum::DownloadAttributes::TransferRate, details.value(Enum::DownloadBackend::TransferRate));
    properties.setValue(Enum::DownloadAttributes::BytesDownloaded, totalBytesDownloaded);
    mDownloadModel->updateDetails(properties);
}

bool DownloadProcessor::compareList(QPair<double, QPair<qint64, QString>> i, QPair<double, QPair<qint64, QString>> j)
{
    return i.first < j.first;
}

void DownloadProcessor::writeToFileInParts()
{
    fetchProperties();
    QByteArray b = properties.getValue(Enum::DownloadAttributes::TempFileNames).toByteArray();
    auto tempFilesMeta = SDM::readByteArray(b);

    Q_ASSERT(!tempFilesMeta.isEmpty());

    for (auto i = tempFilesMeta.begin(); i != tempFilesMeta.end(); ++i) {
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

    properties.setValue(Enum::DownloadAttributes::Status, Enum::Status::Merging);
    mDownloadModel->updateDetails(properties);
    qDebug() << QDir::homePath() + "/sdm/" + filename;
    file.setFileName(QDir::homePath() + "/sdm/" + filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Unable to open file, So opening randomfile";
        file.setFileName(QDir::homePath() + "/sdm/"  + "filename" + QString::number(qrand()));
        file.open(QIODevice::WriteOnly);
    }

    QList<QPair<double, QPair<qint64, QString>>> p;
    for (auto i = tempFilesMeta.begin(); i != tempFilesMeta.end(); ++i) {
        QString filename = i.value().value(3).toString();
        double d = i.key();
        p << QPair<double, QPair<qint64, QString>> (d, QPair<qint64, QString>(i.value().value(1).toLongLong(), filename));
    }
    std::sort(p.begin(), p.end(), compareList);
    for (auto listIt = p.begin(); listIt != p.end(); listIt++) {
        QString filename = (*listIt).second.second;
        QFile tempFile(filename);
        if (!tempFile.open(QIODevice::ReadOnly)) {
            qDebug() << "Error while opening file :" << filename;
            return;
        }
        tempFile.seek(0);
        file.write(tempFile.read((*listIt).second.first));
        Q_ASSERT(file.flush());
    }
    file.close();
    cleanUp();
    emit downloadComplete(id);
    qDebug() << "Write Complete In Parts";

    properties.setValue(Enum::DownloadAttributes::Status, Enum::Status::Completed);
    mDownloadModel->updateDetails(properties);

    b = properties.getValue(Enum::DownloadAttributes::TempFileNames).toByteArray();
    qDebug() << SDM::readByteArray(b);
}

void DownloadProcessor::writeToFileAsWhole()
{
    fetchProperties();
    properties.setValue(Enum::DownloadAttributes::Status, Enum::Status::Merging);
    mDownloadModel->updateDetails(properties);
    qDebug() << QDir::homePath() + "/sdm/" + filename;
    file.setFileName(QDir::homePath() + "/sdm/" + filename);
    if(!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Unable to open file, So opening randomfile";
        file.setFileName(QDir::homePath() + "/sdm/" + "filename" + QString::number(qrand()));
        file.open(QIODevice::WriteOnly);
    }

    dwldaw->tempFile->seek(0);
    file.write(dwldaw->tempFile->readAll());
    file.close();

    properties.setValue(Enum::DownloadAttributes::Status, Enum::Status::Completed);
    properties.setValue(Enum::DownloadAttributes::TransferRate, 0);
    mDownloadModel->updateDetails(properties);

    qDebug() << "Write Complete As Whole";
    cleanUp();
    emit downloadComplete(id);
}

void DownloadProcessor::cleanUp()
{
    if (resumeSupported == true) {
        auto m = SDM::readByteArray(properties.getValue(Enum::DownloadAttributes::TempFileNames).toByteArray());
        for (auto it = m.begin(); it != m.end(); it++) {
            QString filename = it.value().value(3).toString();
            QFile tempFile(filename);
            tempFile.remove();
        }
        for (auto it = dwldip.begin(); it != dwldip.end(); it++) {
            (*it)->deleteLater();
            *it = nullptr;
        }
    } else {
        dwldaw->tempFile->remove();
        dwldaw->deleteLater();
        dwldaw = nullptr;
    }
}

void DownloadProcessor::fetchProperties()
{
    properties = DownloadAttributes(mDownloadModel->getDetails(id));
}

void DownloadProcessor::stopDownload()
{
    if (resumeSupported) {
        QList<Download*>::Iterator it;
        for(it = dwldip.begin(); it != dwldip.end(); it++) {
            if (*it != nullptr) {
                (*it)->abortDownload();
            }
        }
    } else {
        if (dwldaw != nullptr) {
            dwldaw->abortDownload();
        }
    }
    this->deleteLater();
}
