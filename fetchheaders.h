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

#ifndef FETCHHEADERS_H
#define FETCHHEADERS_H

#include "enum.h"
#include "downloadattributes.h"

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
    QString filename();
    int filesize();
    bool fetchHeadersCompleted();
    DownloadAttributes properties();

private:
    DownloadAttributes mProperties;
    QNetworkAccessManager *mNetworkAccessManager;
    QNetworkRequest *mNetworkRequest;
    QNetworkReply *mHeadersReply;
    int mRequestedContentLength;
    int mResponseContentLength;
    int mAlreadyEntered;
    int mOriginalContentLength;
    int mResumeCapability;
    QString mFilename;
    QString mUrlString;
    QString mSizeString;
    bool mHeaderFetchComplete;
    QUrl *mUrl;

    QString parseFilenameFromContentDisposition(QString header);

signals:
    void headersFetched();

private slots:
    void processHeaders(qint64 bytesReceived, qint64 bytesTotal);

    /**
     * Checks if server supports byte-serving and according to that sets resumeCapability. It's
     * been connected to QNetworkReply::downloadProgress signal.
     *
     * Hack: Due to bug in Qt, QNetworkAccessManager::head does not sends the raw headers along
     * with the m_request. So, use QNetworkAccessManager::get and check byte serving.
     */
    void checkByteServing(qint64 bytesReceived, qint64 bytesTotal);
};

#endif // FETCHHEADERS_H
