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

#include "proxymodel.h"
#include "global.h"

#include <QDebug>
#include <QSqlQuery>
#include <QtConcurrent>
#include <QTimer>

ProxyModel::ProxyModel(QObject* parent) : QSortFilterProxyModel(parent)
{
}

DownloadModel* ProxyModel::downloadModel() const
{
    return static_cast<DownloadModel*>(sourceModel());
}

bool ProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        return downloadModel()->setData(mapToSource(index), value, role);
    }
    return false;
}

int ProxyModel::removeDownloadFromModel(int databaseId)
{
    return downloadModel()->removeDownloadFromModel(databaseId);
}

int ProxyModel::insertDownloadIntoModel(DownloadAttributes properties)
{
    return downloadModel()->insertDownloadIntoModel(properties);
}

void ProxyModel::readDatabase()
{
    downloadModel()->readDatabase();
}

const DownloadAttributes* ProxyModel::getDetails(qint64 databaseId)
{
    return downloadModel()->getDetails(databaseId);
}

void ProxyModel::updateDetails(int databaseId, DownloadAttributes properties)
{
    downloadModel()->updateDetails(databaseId, properties);
}

void ProxyModel::updateDetails(DownloadAttributes properties)
{
    downloadModel()->updateDetails(properties.databaseId, properties);
}

int ProxyModel::findRowByDatabaseId(int databaseId) const
{
    int realRowId = downloadModel()->findRowByDatabaseId(databaseId);
    QModelIndex realIndex = downloadModel()->index(realRowId, 0);
    QModelIndex fakeIndex = mapFromSource(realIndex);
    return fakeIndex.row();
}

bool ProxyModel::urlAlreadyInList(QString url)
{
    return downloadModel()->urlAlreadyInList(url);
}

bool ProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    int columnCount = downloadModel()->columnCount(QModelIndex());
    for(int column = 0; column < columnCount; column++) {
        QModelIndex index = downloadModel()->index(sourceRow, column, sourceParent);
        if (downloadModel()->data(index, Qt::DisplayRole).toString().contains(filterRegExp())) {
            return true;
        }
    }
    return false;
}
