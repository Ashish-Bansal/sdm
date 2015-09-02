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

#include "memorydatabase.h"
#include "enum.h"
#include "global.h"
#include "singletonfactory.h"
#include "startdownload.h"

#include <QTimer>
#include <QHash>
#include <QVariant>
#include <QDebug>
#include <QByteArray>
#include <QtConcurrent>
#include <QThreadPool>

MemoryDatabase::MemoryDatabase()
{
    m_dbManager = new DatabaseManager();
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MemoryDatabase::writeToDatabase);
    timer->start(10000);

    connect(this, &MemoryDatabase::downloadInserted, this, &MemoryDatabase::writeToDatabase);
    connect(this, &MemoryDatabase::downloadRemoved, this, &MemoryDatabase::writeToDatabase);
}

MemoryDatabase::~MemoryDatabase()
{
    writeToDatabase();
}

qint64 MemoryDatabase::insertDownload(DownloadAttributes properties)
{
    qint64 id = maxId() + 1;
    if (m_downloadList.value(id, nullptr) == nullptr) {
        qDebug() << "ID duplication found, returning.";
    }
    DownloadAttributes *dld = new DownloadAttributes(properties);
    m_downloadList.insert(id, dld);
    dld->id = id;

    emit downloadInserted(id);
    QtConcurrent::run(m_dbManager, &DatabaseManager::insertDownload,
                      DownloadAttributes(getDetails(id)));
    emit updateGUI(id);
    return id;
}

qint64 MemoryDatabase::maxId()
{
    auto it = m_downloadList.begin();
    qint64 max = 0;
    for(; it != m_downloadList.end(); it++) {
        if (it.key() > max) {
            max = it.key();
        }
    }
    return max;
}

void MemoryDatabase::updateDetails(const DownloadAttributes properties)
{
    qint64 id = properties.id;
    DownloadAttributes *dld = m_downloadList.value(id, nullptr);
    if (dld == nullptr) {
        return;
    }
    dld->id = id;
    dld->filename = properties.filename;
    dld->filesize = properties.filesize;
    dld->resumeCapability = properties.resumeCapability;
    dld->url = properties.url;
    dld->bytesDownloaded = properties.bytesDownloaded;
    dld->transferRate = properties.transferRate;
    dld->status = properties.status;
    dld->tempFileNames = properties.tempFileNames;
    dld->started = properties.started;

    emit updateGUI(id);
}

const DownloadAttributes* MemoryDatabase::getDetails(qint64 id)
{
    auto val = m_downloadList.value(id, nullptr);
    if (val == nullptr) {
        qDebug() << "ID not found";
    }
    return val;
}

void MemoryDatabase::removeDownload(qint64 id)
{
    DownloadAttributes *prop = m_downloadList.value(id, nullptr);
    if (prop == nullptr) {
        qDebug() << "ID not found";
    }

    StartDownload download(id);
    download.cleanUp();
    QtConcurrent::run(m_dbManager, &DatabaseManager::removeDownload, id);
    emit downloadRemoved(id);
}

int MemoryDatabase::restartDownload(qint64 id)
{
    DownloadAttributes *prop = m_downloadList.value(id, nullptr);
    if (prop == nullptr) {
        qDebug() << "ID not found";
        return Enum::SDM::Failed;
    }

    StartDownload download(id);
    download.cleanUp();

    prop->bytesDownloaded = 0;
    prop->started = false;
    prop->transferRate = QString();
    QtConcurrent::run(m_dbManager, &DatabaseManager::updateDetails,
                      DownloadAttributes(getDetails(id)));
    emit updateGUI(id);
    return Enum::SDM::Successful;
}

void MemoryDatabase::writeToDatabase()
{
    auto it = m_downloadList.begin();
    for(; it != m_downloadList.end(); it++) {
            QtConcurrent::run(m_dbManager, &DatabaseManager::updateDetails,
                              DownloadAttributes(getDetails(it.key())));
    }
}

void MemoryDatabase::readDatabase()
{
    QString qryStr = "SELECT * from m_downloadList;";
    QSharedPointer<QSqlQuery> it  = m_dbManager->getIterator(qryStr);

    if (it.isNull()) {
        return;
    }

    while (it->next()) {
        DownloadAttributes *dld = new DownloadAttributes();
        for(int i = 0; i < Enum::DownloadAttributes::END; i++){
            dld->setValue(i, it->value(i));
        }
        dld->setValue(Enum::DownloadAttributes::TransferRate, 0);
        dld->setValue(Enum::DownloadAttributes::Status,
                      dld->status == Enum::Status::Downloading ? Enum::Status::Idle : dld->status);
        m_downloadList.insert(dld->id, dld);
        emit downloadLoaded(it->value(0).toLongLong());
    }
}

void MemoryDatabase::setModel(QSharedPointer<DownloadModel> model)
{
    m_model = model;
}
