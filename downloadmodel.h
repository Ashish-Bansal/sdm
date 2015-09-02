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

#ifndef DOWNLOADMODEL_H
#define DOWNLOADMODEL_H

#include <enum.h>

#include <QObject>
#include <QAbstractTableModel>
#include <QVariant>
#include <QList>

class DownloadModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    DownloadModel();
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());

    struct TableItem{
        qint64 id;
        qint64 rowId;
        QString filename;
        QString filesize;
        QString downloadProgress;
        QString transferRate;
        QString status;
        QString timeRemaining;
        QString dateAdded;
        QString resumeCapability;

        QVariant get(int column)
        {
            switch (column) {
                case Enum::TableView::RowId :
                    return rowId;
                case Enum::TableView::FileName :
                    return filename;
                case Enum::TableView::FileSize :
                    return filesize;
                case Enum::TableView::DownloadProgress :
                    return downloadProgress;
                case Enum::TableView::TransferRate :
                    return transferRate;
                case Enum::TableView::Status :
                    return status;
                case Enum::TableView::TimeRemaining :
                    return timeRemaining;
                case Enum::TableView::DateAdded :
                    return dateAdded;
                default :
                    return QVariant();
            };
        }
    };

    QList< TableItem > getList();
    QList< TableItem > items;
};

#endif // DOWNLOADMODEL_H
