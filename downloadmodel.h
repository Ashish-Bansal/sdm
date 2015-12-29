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

#include "enum.h"
#include "downloadattributes.h"
#include "databasemanager.h"

#include <QObject>
#include <QAbstractTableModel>
#include <QVariant>
#include <QMap>

class DownloadModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    DownloadModel();
    ~DownloadModel();
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    /**
     * Changes some property of DownloadProperties specified by @p index to @p value.
     * Warning: Here row in QModelIndex is Row Id of that element in m_downloadList
     * and column is according to Enum::DownloadAttributes
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);

    void updateDetails(DownloadAttributes properties);
    const DownloadAttributes* getDetails(qint64 id);
    qint64 maxRowId() const;
    void readDatabase();
    int insertDownloadIntoModel(DownloadAttributes properties);
    int removeDownloadFromModel(int databaseId);
    void writeToDatabase();
    int findRowByDatabaseId(int databaseId);
    int loadDownloadIntoModel(DownloadAttributes *properties);
    void deleteDownloadFromDatabase(int id);
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    bool urlAlreadyInList(QString url);

private:
    DatabaseManager *m_dbManager;
    QMap< int, DownloadAttributes* > m_downloadList;
};

#endif // DOWNLOADMODEL_H
