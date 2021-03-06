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

#include "databasemanager.h"

#include <QDateTime>

DatabaseManager::DatabaseManager()
{
    mydb = QSqlDatabase::addDatabase("QSQLITE");
    mydb.setDatabaseName("dmDownloadList");

    if (!mydb.open()) {
        qWarning() << "Unable to open database";
        qWarning() << mydb.lastError();
        return;
    } else {
        qDebug() << "Database opened successfully";
    }

    QSqlQuery qry(mydb);
    QString qryStr = "create table if not exists downloadList(id integer primary key autoincrement, filename varchar, filesize varchar, resumeCapability integer, url varchar, bytesDownloaded integer, tempFileNames blob, date varchar, status integer, transferRate varchar, started integer)";

    if (!qry.exec(qryStr)) {
        qWarning() << "Unable to create table!";
        qWarning() << qry.lastError();
    } else {
        qDebug() << "Table successfully created/opened";
    }
}

DatabaseManager::~DatabaseManager()
{
    qDebug() << "SQL Manager Destructor Called";
}

int DatabaseManager::insertDownload(DownloadAttributes prop)
{
    QSqlQuery qry(mydb);
    QString qryStr = "insert into downloadList values (:id, :filename, :filesize, :resumeCapability, :url, :bytesDownloaded, :tempFileNames, :date, :status, :speed, :started)";
    qry.prepare(qryStr);
    qry.bindValue(":filename", prop.getValue(Enum::DownloadAttributes::Filename));
    qry.bindValue(":filesize", prop.getValue(Enum::DownloadAttributes::Filesize));
    qry.bindValue(":resumeCapability", prop.getValue(Enum::DownloadAttributes::ResumeCapability));
    qry.bindValue(":url", prop.getValue(Enum::DownloadAttributes::Url));
    qry.bindValue(":bytesDownloaded", 0);
    qry.bindValue(":tempFileNames", QByteArray());
    qry.bindValue(":date", QDateTime::currentDateTime().toString());
    qry.bindValue(":status", Enum::Status::Idle);
    qry.bindValue(":speed", prop.getValue(Enum::DownloadAttributes::TransferRate));
    qry.bindValue(":started", 0);
    if (!qry.exec()) {
        qDebug() << qry.lastError();
        Q_ASSERT(false);
        return -1;
    }
    return lastInsertRowId();
}

int DatabaseManager::lastInsertRowId()
{
    QString qryStr = "SELECT last_insert_rowid()";
    QSharedPointer<QSqlQuery> qry = QSharedPointer<QSqlQuery>(new QSqlQuery(mydb));
    if(!qry->exec(qryStr)){
        qWarning() << qry->lastError();
        Q_ASSERT(false);
        return -1;
    }
    qry->first();
    return qry->value(0).toInt();
}

void DatabaseManager::updateDetails(DownloadAttributes prop)
{
    QSqlQuery qry(mydb);
    QString qryStr = "update downloadList set filename = :filename, filesize = :filesize, resumeCapability = :resumeCapability, url = :url, bytesDownloaded = :bytesDownloaded, transferRate = :speed, status = :status, tempFileNames = :tempFileNames, started = :started where id = :id";
    qry.prepare(qryStr);
    qry.bindValue(":id", prop.getValue(Enum::DownloadAttributes::DatabaseId));
    qry.bindValue(":filename", prop.getValue(Enum::DownloadAttributes::Filename));
    qry.bindValue(":filesize", prop.getValue(Enum::DownloadAttributes::Filesize));
    qry.bindValue(":resumeCapability", prop.getValue(Enum::DownloadAttributes::ResumeCapability));
    qry.bindValue(":url", prop.getValue(Enum::DownloadAttributes::Url));
    qry.bindValue(":bytesDownloaded", prop.getValue(Enum::DownloadAttributes::BytesDownloaded));
    qry.bindValue(":speed", prop.getValue(Enum::DownloadAttributes::TransferRate));
    qry.bindValue(":status", prop.getValue(Enum::DownloadAttributes::Status));
    qry.bindValue(":tempFileNames", prop.getValue(Enum::DownloadAttributes::TempFileNames));
    qry.bindValue(":started", prop.getValue(Enum::DownloadAttributes::Started));
    if (!qry.exec()) {
        qWarning() << "Unable to update values!";
        qWarning() << qry.lastError();
        return;
    }
}

void DatabaseManager::removeDownload(qint64 id)
{
    QSqlQuery qry(mydb);
    QString qryStr = "delete from downloadList where id=:id";
    qry.prepare(qryStr);
    qry.bindValue(":id", id);
    if(!qry.exec()){
        qWarning() << "Unable to remove row!";
        qWarning() << qry.lastError();
        return;
    }
}

DownloadAttributes DatabaseManager::getDetails(qint64 id)
{
    QSqlQuery qry(mydb);
    QString qryStr = "select * from downloadList where id=:id";
    qry.prepare(qryStr);
    qry.bindValue(":id", QVariant(id));
    if(!qry.exec()){
        qWarning() << "Unable to retrieve details";
        qWarning() << qry.lastError();
    }
    qry.first();
    DownloadAttributes properties;
    if (!qry.isValid()) {
        qDebug() << "No sql entry found";
        return properties;
    }
    for(int i = 0; i < Enum::DownloadAttributes::END; i++){
        properties.setValue(i, qry.value(i));
    }
    return properties;
}

QSharedPointer<QSqlQuery> DatabaseManager::getIterator(QString qryStr)
{
    QSharedPointer<QSqlQuery> qry = QSharedPointer<QSqlQuery>(new QSqlQuery(mydb));
    if(!qry->exec(qryStr)){
        qWarning() << "Invalid Query";
        qWarning() << qry->lastError();
        return QSharedPointer<QSqlQuery>();
    }
    return qry;
}
