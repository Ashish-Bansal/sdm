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

#include <QDebug>

DownloadAttributes::DownloadAttributes()
{

}

DownloadAttributes::DownloadAttributes(const DownloadAttributes &properties)
    : DownloadAttributes()
{
    id = properties.id;
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
    id = properties.id;
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

void DownloadAttributes::setValue(int i, QVariant v)
{
    if (i == Enum::DownloadAttributes::Id) {
        id = v.toLongLong();
    } else if (i == Enum::DownloadAttributes::Url) {
        url = v.toString();
    } else if (i == Enum::DownloadAttributes::BytesDownloaded) {
        bytesDownloaded = v.toLongLong();
    } else if (i == Enum::DownloadAttributes::Filename) {
        filename = v.toString();
    } else if (i == Enum::DownloadAttributes::Started) {
        started = v.toInt() == 1 ? true : false;
    } else if (i == Enum::DownloadAttributes::Status) {
        status = v.toInt();
    } else if (i == Enum::DownloadAttributes::TransferRate) {
        transferRate = v.toString();
    } else if (i == Enum::DownloadAttributes::tempFileNames) {
        tempFileNames = v.toByteArray();
    } else if (i == Enum::DownloadAttributes::Filesize) {
        filesize = v.toLongLong();
    } else if (i == Enum::DownloadAttributes::ResumeCapability) {
        resumeCapability = v.toInt() == 1 ? true : false;
    }
}
