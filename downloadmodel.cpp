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
    return items.size();
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

    if (index.row() >= items.size() || index.row() < 0) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        TableItem item = items.at(index.row());
        return item.get(index.column());
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
            return tr("Id");
        case Enum::TableView::FileName :
            return tr("Filename");
        case Enum::TableView::FileSize :
            return tr("Size");
        case Enum::TableView::DownloadProgress :
            return tr("Download Progress");
        case Enum::TableView::TransferRate :
            return tr("Transfer Rate");
        case Enum::TableView::Status :
            return tr("Status");
        case Enum::TableView::TimeRemaining :
            return tr("Time Remaining");
        case Enum::TableView::DateAdded :
            return tr("Date Added");
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

        TableItem item = items.value(row);

        switch (index.column()) {
            case Enum::TableView::RowId :
                item.id = value.toInt();
                break;
            case Enum::TableView::FileName :
                item.filename = value.toString();
                break;
            case Enum::TableView::FileSize :
                item.filesize = value.toString();
                break;
            case Enum::TableView::DownloadProgress :
                item.downloadProgress = value.toString();
                break;
            case Enum::TableView::TransferRate :
                item.transferRate = value.toString();
                break;
            case Enum::TableView::Status :
                item.status = value.toString();
                break;
            case Enum::TableView::TimeRemaining :
                item.timeRemaining = value.toString();
                break;
            case Enum::TableView::DateAdded :
                item.dateAdded = value.toString();
                break;
            default:
                return false;
        }
        items.replace(row, item);
        emit(dataChanged(index, index));

        return true;
    }
    return false;
}

bool DownloadModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; row++) {
        TableItem item;
        items.insert(position, item);
    }

    endInsertRows();
    return true;
}

bool DownloadModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        items.removeAt(position);
    }

    endRemoveRows();
    return true;
}

QList< DownloadModel::TableItem > DownloadModel::getList()
{
    return items;
}
