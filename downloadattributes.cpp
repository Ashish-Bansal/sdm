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
