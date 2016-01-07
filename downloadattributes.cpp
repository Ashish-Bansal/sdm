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

DownloadAttributes::DownloadAttributes() :
    mStarted(false),
    mDatabaseId(-1),
    mDownloadProgress(-1),
    mResumeCapability(-1),
    mStatus(-1),
    mFilesize(-1),
    mBytesDownloaded(-1),
    mTimeRemaining(-1),
    mTransferRate(-1)
{
}

DownloadAttributes::DownloadAttributes(const DownloadAttributes &properties)
    : DownloadAttributes()
{
    mDatabaseId = properties.mDatabaseId;
    mFilename = properties.mFilename;
    mFilesize = properties.mFilesize;
    mResumeCapability = properties.mResumeCapability;
    mUrl = properties.mUrl;
    mBytesDownloaded = properties.mBytesDownloaded;
    mTransferRate = properties.mTransferRate;
    mStatus = properties.mStatus;
    mTempFileNames = properties.mTempFileNames;
    mStarted = properties.mStarted;
}

DownloadAttributes::DownloadAttributes(const DownloadAttributes *properties)
    : DownloadAttributes(*properties)
{
}

DownloadAttributes& DownloadAttributes::operator=(const DownloadAttributes& properties)
{
    mDatabaseId = properties.mDatabaseId;
    mFilename = properties.mFilename;
    mFilesize = properties.mFilesize;
    mResumeCapability = properties.mResumeCapability;
    mUrl = properties.mUrl;
    mBytesDownloaded = properties.mBytesDownloaded;
    mTransferRate = properties.mTransferRate;
    mStatus = properties.mStatus;
    mTempFileNames = properties.mTempFileNames;
    mStarted = properties.mStarted;
    mDownloadProgress = properties.mDownloadProgress;
    mTimeRemaining = properties.mTimeRemaining;
    return *this;
}

void DownloadAttributes::setValue(int i, QVariant v)
{
    switch(i) {
        case Enum::DownloadAttributes::RowId :
            break;
        case Enum::DownloadAttributes::DatabaseId :
            mDatabaseId = v.toLongLong();
            break;
        case Enum::DownloadAttributes::Url :
            mUrl = v.toString();
            break;
        case Enum::DownloadAttributes::BytesDownloaded :
            mBytesDownloaded = v.toLongLong();
            break;
        case Enum::DownloadAttributes::Filename :
            mFilename = v.toString();
            break;
        case Enum::DownloadAttributes::Started :
            mStarted = v.toInt() == 1 ? true : false;
            break;
        case Enum::DownloadAttributes::Status :
            mStatus = v.toInt();
            break;
        case Enum::DownloadAttributes::TransferRate :
            mTransferRate = v.toLongLong();
            break;
        case Enum::DownloadAttributes::TempFileNames :
            mTempFileNames = v.toByteArray();
            break;
        case Enum::DownloadAttributes::Filesize :
            mFilesize = v.toLongLong();
            break;
        case Enum::DownloadAttributes::ResumeCapability :
            mResumeCapability = v.toInt() == 1 ? true : false;
            break;
        case Enum::DownloadAttributes::TimeRemaining :
            mTimeRemaining = v.toLongLong();
            break;
        case Enum::DownloadAttributes::DownloadProgress :
            mDownloadProgress = v.toInt();
            break;
        case Enum::DownloadAttributes::DateAdded :
            mDateAdded = v.toString();
            break;
        default:
            qDebug() << "Invalid attribute id :" << i;
            Q_ASSERT(false);
    }
}

QVariant DownloadAttributes::getValue(int i) const
{
    switch(i) {
        case Enum::DownloadAttributes::RowId :
            return -1;
        case Enum::DownloadAttributes::DatabaseId :
            return mDatabaseId;
        case Enum::DownloadAttributes::Url :
            return mUrl;
        case Enum::DownloadAttributes::BytesDownloaded :
            return mBytesDownloaded;
        case Enum::DownloadAttributes::Filename :
            return mFilename;
        case Enum::DownloadAttributes::Started :
            return mStarted;
        case Enum::DownloadAttributes::Status :
            return mStatus;
        case Enum::DownloadAttributes::TransferRate :
            return mTransferRate;
        case Enum::DownloadAttributes::TempFileNames :
            return mTempFileNames;
        case Enum::DownloadAttributes::Filesize :
            return mFilesize;
        case Enum::DownloadAttributes::ResumeCapability :
            return mResumeCapability;
        case Enum::DownloadAttributes::TimeRemaining :
            return mTimeRemaining;
        case Enum::DownloadAttributes::DownloadProgress :
            return mDownloadProgress;
        case Enum::DownloadAttributes::DateAdded :
            return mDateAdded;
        default:
            qDebug() << "Invalid attribute id :" << i;
            Q_ASSERT(false);
    }
}

QVariant DownloadAttributes::getValuesForView(int i) const
{
    switch(i) {
        case Enum::DownloadAttributes::DatabaseId :
            return mDatabaseId;
        case Enum::DownloadAttributes::Url :
            return mUrl;
        case Enum::DownloadAttributes::BytesDownloaded :
            return SDM::convertUnits(mBytesDownloaded);
        case Enum::DownloadAttributes::Filename :
            return mFilename;
        case Enum::DownloadAttributes::Started :
            if (mStarted) {
                return "True";
            } else {
                return "False";
            }
        case Enum::DownloadAttributes::Status :
            switch (mStatus) {
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
            return mTransferRate;
        case Enum::DownloadAttributes::TempFileNames :
            return mTempFileNames;
        case Enum::DownloadAttributes::Filesize :
            return mFilesize != -1 ? SDM::convertUnits(mFilesize) : SDM::convertUnits(mDownloadProgress);
        case Enum::DownloadAttributes::ResumeCapability :
            return mResumeCapability == 1 ? "Yes" : "No";
        case Enum::DownloadAttributes::TimeRemaining :
            if (mTransferRate == 0) {
                return tr("\xE2\x88\x9E");
            }
            return (mFilesize - mBytesDownloaded)/mTransferRate;
        case Enum::DownloadAttributes::DownloadProgress :
            return mFilesize != -1 ? QVariant(mDownloadProgress) : "-";
        case Enum::DownloadAttributes::DateAdded :
            return mDateAdded;
        default:
            qDebug() << "Invalid attribute id :" << i;
            Q_ASSERT(false);
    }
}
