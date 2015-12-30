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

#ifndef PROXYMODEL_H
#define PROXYMODEL_H

#include "enum.h"
#include "downloadattributes.h"
#include "databasemanager.h"
#include "downloadmodel.h"

#include <QObject>
#include <QSortFilterProxyModel>
#include <QVariant>
#include <QMap>

class ProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ProxyModel(QObject *parent = 0);
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool urlAlreadyInList(QString url);
    void updateDetails(DownloadAttributes properties);
    void updateDetails(int databaseId, DownloadAttributes properties);
    int insertDownloadIntoModel(DownloadAttributes properties);
    int removeDownloadFromModel(int databaseId);
    int findRowByDatabaseId(int databaseId) const;
    const DownloadAttributes* getDetails(qint64 id);

private:
    DownloadModel* downloadModel() const;
};

#endif // PROXYMODEL_H
