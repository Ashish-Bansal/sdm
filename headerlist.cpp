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

#include "headerlist.h"
#include "downloadinfodialog.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

HeaderList::HeaderList(QObject *parent) :
    QObject(parent)
{
}

HeaderList::HeaderList(QJsonArray requestHeaders)
{
    processHeadersFromChromium(requestHeaders);
}

void HeaderList::processHeadersFromChromium(QJsonArray requestHeaders)
{
    Q_FOREACH(QJsonValue header, requestHeaders) {
        QJsonObject headerObject = header.toObject();

        Q_ASSERT(headerObject != QJsonObject());

        QString headerName = headerObject.take("name").toString();
        QString headerValue = headerObject.take("value").toString();
        addHeader(headerName, headerValue);
    }
}

int HeaderList::headerCount()
{
    return mHeaders.size();
}

void HeaderList::addHeader(QString headerName, QString headerValue)
{
    mHeaders.insert(headerName, headerValue);
}

void HeaderList::removeHeader(QString headerName)
{
    Q_ASSERT(!mHeaders.contains(headerName));
    mHeaders.remove(headerName);
}

QMap< QString, QString > HeaderList::rawHeaders()
{
    return mHeaders;
}
