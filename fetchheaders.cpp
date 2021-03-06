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

#include "fetchheaders.h"
#include "global.h"

#include <QMessageBox>
#include <QStandardPaths>

FetchHeaders::FetchHeaders(QString rawUrl) :
    mRequestedContentLength(0),
    mResponseContentLength(0),
    mOriginalContentLength(0),
    mResumeCapability(Enum::SDM::ResumeNotSupported),
    mUrlString(rawUrl),
    mHeaderFetchComplete(false)
{
    mFilename = SDM::filenameFromUrl(mUrlString);
    mProperties.setValue(Enum::DownloadAttributes::Filename, mFilename);

    mNetworkAccessManager = new QNetworkAccessManager();

    mProperties.setValue(Enum::DownloadAttributes::Url, mUrlString);
    mUrl = new QUrl(mUrlString);

    mNetworkRequest = new QNetworkRequest();
    mNetworkRequest->setUrl(*mUrl);
}

FetchHeaders::FetchHeaders(QString rawUrl, HeaderList headers) :
    FetchHeaders(rawUrl)
{
    mHeaders = headers;
}

FetchHeaders::~FetchHeaders()
{
    qDebug() << "FetchHeaders Destructor called!";
}

void FetchHeaders::fetchHeaders()
{
    auto headerMap = mHeaders.rawHeaders();
    auto it = headerMap.begin();
    for(; it != headerMap.end(); it++) {
        mNetworkRequest->setRawHeader(it.key().toLocal8Bit(), it.value().toLocal8Bit());
    }

    mNetworkRequest->setRawHeader(QByteArray("Range"), QByteArray("bytes=0-1"));
    mRequestedContentLength = 2;

    mHeadersReply = mNetworkAccessManager->get(*mNetworkRequest);
    connect(mHeadersReply, &QNetworkReply::downloadProgress,
            this, &FetchHeaders::checkByteServing);
}

void FetchHeaders::checkByteServing(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesReceived);

    int statusCode = mHeadersReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    mResponseContentLength = bytesTotal;
    qDebug() << mUrlString;
    qDebug() << "Status Code :" << statusCode;
    qDebug() << "Bytes Total :" << bytesTotal ;
    qDebug() << mHeadersReply->rawHeaderPairs();
    if (statusCode == 301 || statusCode == 302 || statusCode == 303
            || statusCode == 307 || statusCode == 308) {
        qDebug() << "Redirection";
        mUrl->setUrl(mHeadersReply->rawHeader("Location"));
        mUrlString = mUrl->toString();
        mFilename = SDM::filenameFromUrl(mUrlString);
        mNetworkRequest->setUrl(*mUrl);
        mNetworkRequest->setRawHeader("Range", "bytes=0-1");
        mHeadersReply = mNetworkAccessManager->get(*mNetworkRequest);
        return;
    } else if (statusCode/100 == 4) {
        //ToDo: Close Download Dialog if error
        QMessageBox::warning(nullptr, "Unable to download file",
                             "Provided URL cannot be downloaded!");
        return;
    }

    if(statusCode == 206 || mResponseContentLength == mRequestedContentLength) {
        mResumeCapability = Enum::SDM::ResumeSupported;
    } else {
        mResumeCapability = Enum::SDM::ResumeNotSupported;
    }

    mHeadersReply->abort();
    disconnect(mHeadersReply, &QNetworkReply::downloadProgress,
               this, &FetchHeaders::checkByteServing);

    mNetworkRequest->setUrl(*mUrl);

    auto headerMap = mHeaders.rawHeaders();
    auto it = headerMap.begin();
    for(; it != headerMap.end(); it++) {
        mNetworkRequest->setRawHeader(it.key().toLocal8Bit(), it.value().toLocal8Bit());
    }

    mNetworkRequest->setRawHeader("Range", "bytes=0-");
    mHeadersReply = mNetworkAccessManager->head(*mNetworkRequest);
    connect(mHeadersReply, &QNetworkReply::downloadProgress,
            this, &FetchHeaders::processHeaders);
}

void FetchHeaders::processHeaders(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesReceived);

    QString contentRangeHeader = QString(mHeadersReply->rawHeader("Content-Range"));
    qint64 contentLength = mHeadersReply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
    if (contentLength) {
        mOriginalContentLength = contentLength;
    } else if (contentRangeHeader.length() && contentRangeHeader.contains("/")) {
        mOriginalContentLength = mHeadersReply->rawHeader(
                                     "Content-Range").split('/').at(1).toLongLong(0);
    } else if (bytesTotal > mRequestedContentLength) {
        mOriginalContentLength = bytesTotal;
    } else {
        mOriginalContentLength = -1;
    }

    QString contentDispositionHeader = QString(mHeadersReply->rawHeader("Content-Disposition"));

    QString possibleFilename = parseFilenameFromContentDisposition(contentDispositionHeader);
    if (!possibleFilename.isEmpty()) {
        mFilename = possibleFilename;
    }

    qDebug() << "Filename :" << mFilename;

    mProperties.setValue(Enum::DownloadAttributes::Filename, mFilename);
    mProperties.setValue(Enum::DownloadAttributes::Filesize, mOriginalContentLength);
    mProperties.setValue(Enum::DownloadAttributes::ResumeCapability, mResumeCapability);
    mProperties.setValue(Enum::DownloadAttributes::Url, mUrlString);
    mSizeString = SDM::convertUnits(mOriginalContentLength);

    mHeaderFetchComplete = true;
    emit headersFetched();
}

QString FetchHeaders::filename()
{
    return mFilename;
}

int FetchHeaders::filesize()
{
    return mOriginalContentLength;
}

bool FetchHeaders::fetchHeadersCompleted()
{
    return mHeaderFetchComplete;
}

DownloadAttributes FetchHeaders::properties()
{
    return mProperties;
}

QString FetchHeaders::parseFilenameFromContentDisposition(QString header)
{
    int indexOfEqual = header.indexOf('=');
    if (indexOfEqual == -1) {
        return QString();
    }

    QString filename = header.mid(indexOfEqual + 1);
    if (filename.at(0) == '"' && filename.at(filename.size() - 1) == '"') {
        filename = filename.mid(1, filename.size() - 2);
    }
    return filename;
}
