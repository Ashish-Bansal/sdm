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
    return Enum::TableView::END;
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
        return item->getValue(index.column());
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
        case Enum::TableView::RowId :
            return tr("Row Id");
        case Enum::TableView::DatabaseId :
            return tr("Database Id");
        case Enum::TableView::Filename :
            return tr("Filename");
        case Enum::TableView::Filesize :
            return tr("Size");
        case Enum::TableView::ResumeCapability :
            return tr("Resume Capability");
        case Enum::TableView::BytesDownloaded :
            return tr("Downloaded");
        case Enum::TableView::DateAdded :
            return tr("Date Added");
        case Enum::TableView::Status :
            return tr("Status");
        case Enum::TableView::TransferRate :
            return tr("Transfer Rate");
        case Enum::TableView::TimeRemaining :
            return tr("Time Remaining");
        case Enum::TableView::DownloadProgress :
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
        int row = index.row();

        DownloadAttributes *item = m_downloadList.value(row, nullptr);
        Q_ASSERT(item != nullptr);

        // Warning: Here column needs to be according to Enum::DownloadAttributes instead of Enum::TableView
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
    m_downloadList.remove(rowId);
    endRemoveRows();
    writeToDatabase();
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

qint64 DownloadModel::maxRowId()
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
        dld->databaseId = it->value("id").toInt();
        dld->filename = it->value("filename").toString();
        dld->filesize = it->value("filesize").toInt();
        dld->started = it->value("started").toInt();
        dld->resumeCapability = it->value("resumeCapability").toInt();
        dld->url = it->value("url").toString();
        dld->bytesDownloaded = it->value("bytesDownloaded").toInt();
        dld->dateAdded = it->value("date").toString();
        dld->tempFileNames = it->value("tempFileNames").toByteArray();
        dld->setValue(Enum::DownloadAttributes::TransferRate, 0);
        dld->setValue(Enum::DownloadAttributes::Status,
                      dld->status == Enum::Status::Downloading ? Enum::Status::Idle : dld->status);
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
