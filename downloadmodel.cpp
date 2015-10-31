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

#include <QDebug>

DownloadModel::DownloadModel() : QAbstractTableModel()
{

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
        DownloadAttributes *item = m_downloadList[index.row()];
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

        switch (index.column()) {
            case Enum::TableView::RowId :
                item->rowId = value.toInt();
                break;
            case Enum::TableView::DatabaseId :
                item->databaseId = value.toInt();
                break;
            case Enum::TableView::Filename :
                item->filename = value.toString();
                break;
            case Enum::TableView::Filesize :
                item->filesize = value.toLongLong();
                break;
            case Enum::TableView::DownloadProgress :
                item->downloadProgress = value.toInt();
                break;
            case Enum::TableView::TransferRate :
                item->transferRate = value.toString();
                break;
            case Enum::TableView::Status :
                item->status = value.toInt();
                break;
            case Enum::TableView::TimeRemaining :
                item->timeRemaining = value.toLongLong();
                break;
            case Enum::TableView::DateAdded :
                item->dateAdded = value.toString();
                break;
            default:
                return false;
        }
        emit(dataChanged(index, index));

        return true;
    }
    return false;
}

int DownloadModel::removeDownloadFromModel(int databaseId)
{
    int position = -1;
    foreach(DownloadAttributes *item, m_downloadList) {
        if (item->databaseId == databaseId) {
            position = item->rowId;
        }
    }

    Q_ASSERT(position != -1);

    beginRemoveRows(QModelIndex(), position, position);
    m_downloadList.remove(position);
    endRemoveRows();
    return databaseId;
}

int DownloadModel::insertDownloadIntoModel(DownloadAttributes *properties)
{
    int position = maxRowId();
    beginInsertRows(QModelIndex(), position, position);
    m_downloadList.insert(position+1, properties);
    endInsertRows();
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
