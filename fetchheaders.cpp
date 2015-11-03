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

FetchHeaders::FetchHeaders(QString rawURL)
{
     m_qnam = new QNetworkAccessManager();

     stringUrl = rawURL;
     properties.url = stringUrl;
     url = new QUrl(stringUrl);

     m_req = new QNetworkRequest();
     m_req->setUrl(*url);
     m_req->setRawHeader(QByteArray("Range"), QByteArray("bytes=0-1"));
     m_requestedContentLength = 2;

     m_headersCheckReply = m_qnam->get(*m_req);
     connect(m_headersCheckReply, &QNetworkReply::downloadProgress,
             this, &FetchHeaders::checkByteServing);
}

FetchHeaders::~FetchHeaders()
{
    qDebug() << "FetchHeaders Destructor called!";
}

void FetchHeaders::checkByteServing(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesReceived);


    int statusCode = m_headersCheckReply->attribute(
                QNetworkRequest::HttpStatusCodeAttribute).toInt();
    m_responseContentLength = bytesTotal;
    qDebug() << stringUrl;
    qDebug() << "Status Code :" << statusCode;
    qDebug() << "Bytes Total :" << bytesTotal ;
    qDebug() << m_headersCheckReply->rawHeaderPairs();
    if (statusCode == 301 || statusCode == 302 || statusCode == 303
            || statusCode == 307 || statusCode == 308) {
        qDebug() << "Redirection";
        url->setUrl(m_headersCheckReply->rawHeader("Location"));
        stringUrl = url->toString();
        m_req->setUrl(*url);
        m_req->setRawHeader("Range", "bytes=0-1");
        m_headersCheckReply = m_qnam->get(*m_req);
        connect(m_headersCheckReply, &QNetworkReply::downloadProgress,
                this, &FetchHeaders::processHeaders);
        return;
    } else if (statusCode/100 == 4) {
        //ToDo: Close Download Dialog if error
        QMessageBox::warning(nullptr, "Unable to download file",
                             "Provided URL cannot be downloaded!");
        return;
    }

    if(statusCode == 206 || m_responseContentLength == m_requestedContentLength) {
        resumeCapability = Enum::SDM::ResumeSupported;
    } else{
        resumeCapability = Enum::SDM::ResumeNotSupported;
    }

    m_headersCheckReply->abort();
    disconnect(m_headersCheckReply, &QNetworkReply::downloadProgress,
             this, &FetchHeaders::checkByteServing);

    m_req->setUrl(*url);
    m_req->setRawHeader("Range", "bytes=0-");
    m_headersCheckReply = m_qnam->head(*m_req);
    connect(m_headersCheckReply, &QNetworkReply::downloadProgress,
            this, &FetchHeaders::processHeaders);
}

void FetchHeaders::processHeaders(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesReceived);
    QString contentRangeHeader = QString(m_headersCheckReply->rawHeader("Content-Range"));
    qint64 contentLength = m_headersCheckReply->header(
                QNetworkRequest::ContentLengthHeader).toLongLong();
    if (contentLength) {
        originalContentLength = contentLength;
    } else if (contentRangeHeader.length() && contentRangeHeader.contains("/")) {
        originalContentLength = m_headersCheckReply->rawHeader(
                    "Content-Range").split('/').at(1).toLongLong(0);
    } else if (bytesTotal > m_requestedContentLength) {
        originalContentLength = bytesTotal;
    } else {
        originalContentLength = -1;
    }

    QString contentDispositionHeader = QString(m_headersCheckReply->rawHeader(
                                                   "Content-Disposition"));
    if (contentDispositionHeader.length() && contentDispositionHeader.contains("filename")) {
        fileName = contentDispositionHeader.split("filename=\"").at(1).split("\"").at(0);
        qDebug() << "FileName :" << fileName;
    } else {
        fileName = SDM::filenameFromUrl(*url);
        qDebug() << fileName;
    }

    properties.filename = fileName;
    properties.filesize = originalContentLength;
    properties.resumeCapability = resumeCapability;
    properties.url = url->toString();
    size = SDM::convertUnits(originalContentLength);

    headerFetchComplete = true;
    emit headersFetched();
}
