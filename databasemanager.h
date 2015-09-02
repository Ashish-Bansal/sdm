/***********************************************************************************
 *   Copyright 2015 Ashish Bansal<bansal.ashish096@gmail.com>                      *
 *                                                                                 *
 *                                                                                 *
 *   This library is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public                    *
 *   License as published by the Free Software Foundation; either                  *
 *   version 2.1 of the License, or (at your option) any later version.            *
 *                                                                                 *
 *   This library is distributed in the hope that it will be useful,               *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of                *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
 *   Lesser General Public License for more details.                               *
 *                                                                                 *
 *   You should have received a copy of the GNU Lesser General Public              *
 *   License along with this library.  If not, see <http://www.gnu.org/licenses/>. *
 ***********************************************************************************/

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "enum.h"
#include "downloadattributes.h"

#include <QString>
#include <QObject>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QDebug>
#include <QSharedPointer>
#include <QSqlError>

class DatabaseManager :public QObject
{
    Q_OBJECT

public:
    DatabaseManager();
    ~DatabaseManager();
    QSqlDatabase mydb;
    void insertDownload(DownloadAttributes properties);
    void removeDownload(qint64 id);
    int lastInsertRowId();
    void updateDetails(DownloadAttributes properties);
    DownloadAttributes getDetails(qint64 id);
    QSharedPointer<QSqlQuery> getIterator(QString qryStr);

};

#endif // DATABASEMANAGER_H
