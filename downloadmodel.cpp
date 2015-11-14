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

#include "downloadmodel.h"
#include "global.h"

#include <QDebug>
#include <QSqlQuery>
#include <QtConcurrent>
#include <QTimer>

DownloadModel::DownloadModel() : QAbstractTableModel()
{
    m_dbManager = new DatabaseManager();
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DownloadModel::writeToDatabase);
    timer->start(10000);
}

DownloadModel::~DownloadModel()
{
    writeToDatabase();
}

int DownloadModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_downloadList.size();
}

int DownloadModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return Enum::DownloadAttributes::END;
}

QVariant DownloadModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_downloadList.size() || index.row() < 0) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        DownloadAttributes *item = m_downloadList.value(index.row() + 1, nullptr);
        Q_ASSERT(item != nullptr);
        if (index.column() == Enum::DownloadAttributes::RowId) {
            return index.row() + 1;
        }
        return item->getValuesForView(index.column());
    }

    return QVariant();
}

QVariant DownloadModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case Enum::DownloadAttributes::RowId :
            return tr("#");
        case Enum::DownloadAttributes::Filename :
            return tr("Name");
        case Enum::DownloadAttributes::Filesize :
            return tr("Size");
        case Enum::DownloadAttributes::ResumeCapability :
            return tr("Resume Capability");
        case Enum::DownloadAttributes::BytesDownloaded :
            return tr("Done");
        case Enum::DownloadAttributes::DateAdded :
            return tr("Date Added");
        case Enum::DownloadAttributes::Status :
            return tr("Status");
        case Enum::DownloadAttributes::TransferRate :
            return tr("Download Speed");
        case Enum::DownloadAttributes::TimeRemaining :
            return tr("ETA");
        case Enum::DownloadAttributes::DownloadProgress :
            return tr("Progress");
        default:
            return QVariant();
        }
    }
    return QVariant();
}

Qt::ItemFlags DownloadModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::ItemIsEnabled;
    }
    return QAbstractTableModel::flags(index);
}

bool DownloadModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        int rowId = index.row();

        DownloadAttributes *item = m_downloadList.value(rowId, nullptr);
        Q_ASSERT(item != nullptr);

        item->setValue(index.column(), value);
        emit(dataChanged(index, index));

        return true;
    }
    return false;
}

int DownloadModel::removeDownloadFromModel(int databaseId)
{
    int rowId = findRowByDatabaseId(databaseId);
    beginRemoveRows(QModelIndex(), rowId, rowId);
    m_downloadList[rowId]->deleteLater();
    m_downloadList.remove(rowId);
    endRemoveRows();
    deleteDownloadFromDatabase(databaseId);
    return databaseId;
}

int DownloadModel::insertDownloadIntoModel(DownloadAttributes *properties)
{
    properties->databaseId = m_dbManager->insertDownload(properties);
    loadDownloadIntoModel(properties);
    writeToDatabase();
    return properties->databaseId;
}

int DownloadModel::loadDownloadIntoModel(DownloadAttributes *properties)
{
    int lastRow = maxRowId();
    beginInsertRows(QModelIndex(), lastRow, lastRow);
    foreach(DownloadAttributes *item, m_downloadList) {
        Q_ASSERT(item->databaseId != properties->databaseId);
    }
    m_downloadList.insert(lastRow+1, properties);
    endInsertRows();
    return properties->databaseId;
}

qint64 DownloadModel::maxRowId() const
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

void DownloadModel::readDatabase()
{
    QString qryStr = "SELECT * from downloadList;";
    QSharedPointer<QSqlQuery> it  = m_dbManager->getIterator(qryStr);

    if (it.isNull()) {
        return;
    }

    while (it->next()) {
        DownloadAttributes *dld = new DownloadAttributes();

        Q_ASSERT(it->value("id").isValid());
        dld->databaseId = it->value("id").toInt();
        Q_ASSERT(it->value("filename").isValid());
        dld->filename = it->value("filename").toString();
        Q_ASSERT(it->value("filesize").isValid());
        dld->filesize = it->value("filesize").toLongLong();
        Q_ASSERT(it->value("started").isValid());
        dld->started = it->value("started").toInt();
        Q_ASSERT(it->value("resumeCapability").isValid());
        dld->resumeCapability = it->value("resumeCapability").toInt();
        Q_ASSERT(it->value("url").isValid());
        dld->url = it->value("url").toString();
        Q_ASSERT(it->value("bytesDownloaded").isValid());
        dld->bytesDownloaded = it->value("bytesDownloaded").toInt();
        Q_ASSERT(it->value("date").isValid());
        dld->dateAdded = it->value("date").toString();
        Q_ASSERT(it->value("tempFileNames").isValid());
        dld->tempFileNames = it->value("tempFileNames").toByteArray();
        Q_ASSERT(it->value("status").isValid());
        dld->status = it->value("status").toInt();
        dld->setValue(Enum::DownloadAttributes::TransferRate, 0);
        loadDownloadIntoModel(dld);
    }
}

void DownloadModel::writeToDatabase()
{
    auto it = m_downloadList.begin();
    for(; it != m_downloadList.end(); it++) {
        QtConcurrent::run(m_dbManager, &DatabaseManager::updateDetails,
                          DownloadAttributes(m_downloadList[it.key()]));
    }
}

void DownloadModel::deleteDownloadFromDatabase(int id)
{
    QtConcurrent::run(m_dbManager, &DatabaseManager::removeDownload,
                      id);
}

const DownloadAttributes* DownloadModel::getDetails(qint64 databaseId)
{
    int rowId = findRowByDatabaseId(databaseId);
    return m_downloadList[rowId];
}

void DownloadModel::updateDetails(DownloadAttributes properties)
{
    int rowId = findRowByDatabaseId(properties.databaseId);
    DownloadAttributes *item = m_downloadList[rowId];
    for(int i=0; i < Enum::DownloadAttributes::END; i++) {
        item->setValue(i, properties.getValue(i));
    }
}

int DownloadModel::findRowByDatabaseId(int databaseId)
{
    int position = -1;
    foreach(DownloadAttributes *item, m_downloadList) {
        if (item->databaseId == databaseId) {
            position = m_downloadList.key(item);
            break;
        }
    }

    Q_ASSERT(position != -1);

    return position;
}

QModelIndex DownloadModel::index(int row, int column, const QModelIndex &parent) const {
    if (maxRowId() >= row && Enum::DownloadAttributes::END > column) {
        return this->createIndex(row, column);
    }
    return QModelIndex();
}

/*

int DownloadModel::restartDownload(qint64 id)
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

void DownloadModel::removeDownload(qint64 id)
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

*/
