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

#include "global.h"
#include "downloadproperties.h"

#include <QRegExp>
#include <QtMath>
#include <QLocale>
#include <QDebug>

bool SDM::validateUrl(QString url)
{
    QString regexp = "^(((http|ftp)(s?)\\:\\/\\/)|(www\\.))(([a-zA-Z0-9\\-\\._]+(\\.[a-zA-Z0-9\\-\\._]+)+)|localhost)(\\/?)([a-zA-Z0-9\\-\\.\\?\\,\\'\\/\\\\\\+&amp;%\\$#_]*)?([\\d\\w\\.\\/\\%\\+\\-\\=\\&amp;\\?\\:\\\\\\&quot;\\'\\,\\|\\~\\;]*)$";
    QRegExp rx(regexp);
    rx.indexIn(url);
    return rx.cap(0).length() != 0;
}

QString SDM::convertUnits(qint64 bytes)
{
    if (bytes <= 0) {
        return "unknown";
    }
    QString units[5] = {"B", "KB", "MB", "GB", "TB"};
    int pow = floor(log((double)bytes)/log(1024));
    pow = qMin(pow, 5);
    QString fileSize = QLocale().toString(bytes/qPow(1024, pow), 'g', 2) + units[pow];
    return fileSize;
}

QDebug operator<<(QDebug d, const DownloadProperties prop)
{
    d << "id" << prop.id;
    d << "filename" << prop.filename;
    d << "filesize" << prop.filesize;
    d << "resumeCapability" << prop.resumeCapability;
    d << "url" << prop.url;
    d << "bytesDownloaded" << prop.bytesDownloaded;
    d << "transferRate" << prop.transferRate;
    d << "status" << prop.status;
    d << "tempFileNames" << prop.tempFileNames;
    d << "started" << prop.started;
    return d;
}

QMultiMap <double, QMultiMap <qint8, QVariant> > SDM::readByteArray(QByteArray b)
{
    QDataStream dsOut(&b, QIODevice::ReadOnly);
    QMultiMap <double, QMultiMap <qint8, QVariant> > savedFilesMeta;
    dsOut >> savedFilesMeta;
    return savedFilesMeta;
}


QByteArray SDM::writeToByteArray(QMultiMap<double, QMultiMap<qint8, QVariant> > m)
{
    QByteArray b;
    QDataStream dsIn(&b, QIODevice::WriteOnly);
    dsIn << m;
    return b;
}
