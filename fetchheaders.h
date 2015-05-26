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

#ifndef FETCHHEADERS_H
#define FETCHHEADERS_H

#include "enum.h"
#include "downloadproperties.h"

#include <QObject>
#include <QUrl>
#include <QString>
#include <QVariant>
#include <QHash>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

class FetchHeaders : public QObject
{
    Q_OBJECT
public:
    FetchHeaders(QString rawURL);
    ~FetchHeaders();
    QString fileName;
    QString size;
    QString convertUnits(qint64 bytes);
    int resumeCapability;
    QUrl *url;
    QString stringUrl;
    int originalContentLength;
    DownloadProperties properties;
    bool headerFetchComplete;

private:
    QNetworkAccessManager *qnam;
    QNetworkRequest *req;
    QNetworkReply *headersCheckReply;
    int requestedContentLength;
    int responseContentLength;
    int mAlreadyEntered;

signals:
    void headersFetched();

private slots:
    void processHeaders(qint64 bytesReceived, qint64 bytesTotal);
    void checkByteServing(qint64 bytesReceived, qint64 bytesTotal);
};

#endif // FETCHHEADERS_H
