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

#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QByteArray>
#include <QMultiMap>
#include <QString>
#include <QByteArray>
#include <QVariant>

namespace SDM
{
    /* Copyright (c) 2010-2013 Diego Perini (http://www.iport.it)
     * https://gist.github.com/dperini/729294
     */
    const QString regexp = "^(?:(?:https?|ftp):\\/\\/)(?:\\S+(?::\\S*)?@)?(?:(?!(?:10|127)(?:\\.\\d{1,3}){3})(?!(?:169\\.254|192\\.168)(?:\\.\\d{1,3}){2})(?!172\\.(?:1[6-9]|2\\d|3[0-1])(?:\\.\\d{1,3}){2})(?:[1-9]\\d?|1\\d\\d|2[01]\\d|22[0-3])(?:\\.(?:1?\\d{1,2}|2[0-4]\\d|25[0-5])){2}(?:\\.(?:[1-9]\\d?|1\\d\\d|2[0-4]\\d|25[0-4]))|(?:(?:[a-z0-9]-*)*[a-z0-9]+)(?:\\.(?:[a-z0-9]-*)*[a-z0-9]+)*(?:\\.(?:[a-z]{2,}))\\.?)(?::\\d{2,5})?(?:[/?#]\\S*)?$";

    bool isValidUrl(QString url);
    QString convertUnits(qint64 bytes);
    QMultiMap<double, QMultiMap<qint8, QVariant> > readByteArray(QByteArray b);
    QByteArray writeToByteArray(QMultiMap <double, QMultiMap <qint8, QVariant> > m);
    QString filenameFromUrl(QString url);
    QString filenameFromUrl(QUrl url);
}

#endif // GLOBAL_H
