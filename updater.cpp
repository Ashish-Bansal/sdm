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

#include "updater.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QApplication>

Updater::Updater(QObject *parent) :
    QObject(parent),
    mUpdateUrl("https://raw.githubusercontent.com/Ashish-Bansal/sdm/master/release")
{
    mNetworkAccessManager = new QNetworkAccessManager(this);
    connect(mNetworkAccessManager, &QNetworkAccessManager::finished,
            this, &Updater::processReply);
    mNetworkAccessManager->get(QNetworkRequest(mUpdateUrl));
}

void Updater::processReply(QNetworkReply *reply)
{
    QByteArray data = reply->readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(data));
    QJsonObject obj = loadDoc.object();
    QString applicationVersion = obj["version"].toString();
    qDebug() << "Application version :" << applicationVersion;
    if (QApplication::applicationVersion() != applicationVersion) {
        //ToDo: Show a dialog box to download latest binary.
    }
}
