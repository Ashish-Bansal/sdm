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

#include "downloadattributes.h"
#include "enum.h"
#include "global.h"

#include <QDebug>

DownloadAttributes::DownloadAttributes()
{

}

DownloadAttributes::DownloadAttributes(const DownloadAttributes &properties)
    : DownloadAttributes()
{
    databaseId = properties.databaseId;
    filename = properties.filename;
    filesize = properties.filesize;
    resumeCapability = properties.resumeCapability;
    url = properties.url;
    bytesDownloaded = properties.bytesDownloaded;
    transferRate = properties.transferRate;
    status = properties.status;
    tempFileNames = properties.tempFileNames;
    started = properties.started;
}

DownloadAttributes::DownloadAttributes(const DownloadAttributes *properties)
    : DownloadAttributes(*properties)
{
}

DownloadAttributes& DownloadAttributes::operator=(const DownloadAttributes& properties)
{
    databaseId = properties.databaseId;
    filename = properties.filename;
    filesize = properties.filesize;
    resumeCapability = properties.resumeCapability;
    url = properties.url;
    bytesDownloaded = properties.bytesDownloaded;
    transferRate = properties.transferRate;
    status = properties.status;
    tempFileNames = properties.tempFileNames;
    started = properties.started;
    downloadProgress = properties.downloadProgress;
    timeRemaining = properties.timeRemaining;
}

void DownloadAttributes::setValue(int i, QVariant v)
{
    switch(i) {
        case Enum::DownloadAttributes::DatabaseId :
            databaseId = v.toLongLong();
            break;
        case Enum::DownloadAttributes::Url :
            url = v.toString();
            break;
        case Enum::DownloadAttributes::BytesDownloaded :
            bytesDownloaded = v.toLongLong();
            break;
        case Enum::DownloadAttributes::Filename :
            filename = v.toString();
            break;
        case Enum::DownloadAttributes::Started :
            started = v.toInt() == 1 ? true : false;
            break;
        case Enum::DownloadAttributes::Status :
            status = v.toInt();
            break;
        case Enum::DownloadAttributes::TransferRate :
            transferRate = v.toLongLong();
            break;
        case Enum::DownloadAttributes::TempFileNames :
            tempFileNames = v.toByteArray();
            break;
        case Enum::DownloadAttributes::Filesize :
            filesize = v.toLongLong();
            break;
        case Enum::DownloadAttributes::ResumeCapability :
            resumeCapability = v.toInt() == 1 ? true : false;
            break;
        case Enum::DownloadAttributes::TimeRemaining :
            timeRemaining = v.toLongLong();
            break;
        case Enum::DownloadAttributes::DownloadProgress :
            downloadProgress = v.toInt();
            break;
        case Enum::DownloadAttributes::DateAdded :
            dateAdded = v.toString();
            break;
        default:
            qDebug() << "Invalid attribute id :" << i;
            Q_ASSERT(false);
    }
}

QVariant DownloadAttributes::getValue(int i)
{
    switch(i) {
        case Enum::DownloadAttributes::DatabaseId :
            return databaseId;
        case Enum::DownloadAttributes::Url :
            return url;
        case Enum::DownloadAttributes::BytesDownloaded :
            return bytesDownloaded;
        case Enum::DownloadAttributes::Filename :
            return filename;
        case Enum::DownloadAttributes::Started :
            return started;
        case Enum::DownloadAttributes::Status :
            return status;
        case Enum::DownloadAttributes::TransferRate :
            return transferRate;
        case Enum::DownloadAttributes::TempFileNames :
            return tempFileNames;
        case Enum::DownloadAttributes::Filesize :
            return filesize;
        case Enum::DownloadAttributes::ResumeCapability :
            return resumeCapability;
        case Enum::DownloadAttributes::TimeRemaining :
            return timeRemaining;
        case Enum::DownloadAttributes::DownloadProgress :
            return downloadProgress;
        case Enum::DownloadAttributes::DateAdded :
            return dateAdded;
        default:
            qDebug() << "Invalid attribute id :" << i;
            Q_ASSERT(false);
    }
}

QVariant DownloadAttributes::getValuesForView(int i)
{
    switch(i) {
        case Enum::DownloadAttributes::DatabaseId :
            return databaseId;
        case Enum::DownloadAttributes::Url :
            return url;
        case Enum::DownloadAttributes::BytesDownloaded :
            return bytesDownloaded;
        case Enum::DownloadAttributes::Filename :
            return filename;
        case Enum::DownloadAttributes::Started :
            if (started) {
                return "True";
            } else {
                return "False";
            }
        case Enum::DownloadAttributes::Status :
            switch (status) {
                case Enum::Status::Idle :
                    return tr("Idle");
                case Enum::Status::Completed :
                    return tr("Completed");
                case Enum::Status::Downloading :
                    return tr("Downloading");
                case Enum::Status::Error :
                    return tr("Error");
                case Enum::Status::Merging :
                    return tr("Merging");
            }
        case Enum::DownloadAttributes::TransferRate :
            return transferRate;
        case Enum::DownloadAttributes::TempFileNames :
            return tempFileNames;
        case Enum::DownloadAttributes::Filesize :
            return filesize;
        case Enum::DownloadAttributes::ResumeCapability :
            return resumeCapability;
        case Enum::DownloadAttributes::TimeRemaining :
            if (transferRate == 0) {
                return tr("\xE2\x88\x9E");
            }
            return (filesize - bytesDownloaded)/transferRate;
        case Enum::DownloadAttributes::DownloadProgress :
            return SDM::convertUnits(bytesDownloaded);
        case Enum::DownloadAttributes::DateAdded :
            return dateAdded;
        default:
            qDebug() << "Invalid attribute id :" << i;
            Q_ASSERT(false);
    }
}
