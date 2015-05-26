#include "downloadproperties.h"
#include "enum.h"

#include <QDebug>

DownloadProperties::DownloadProperties()
{

}

DownloadProperties::DownloadProperties(const DownloadProperties &properties)
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

DownloadProperties::DownloadProperties(const DownloadProperties *properties)
{
//    qDebug() << properties->id;
    id = properties->id;
    filename = properties->filename;
    filesize = properties->filesize;
    resumeCapability = properties->resumeCapability;
    url = properties->url;
    bytesDownloaded = properties->bytesDownloaded;
    transferRate = properties->transferRate;
    status = properties->status;
    tempFileNames = properties->tempFileNames;
    started = properties->started;
}

DownloadProperties& DownloadProperties::operator=(const DownloadProperties& properties)
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

void DownloadProperties::setValue(int i, QVariant v)
{
    qDebug() << v;
    if (i == DownloadAttributes::Id) {
        id = v.toLongLong();
    } else if (i == DownloadAttributes::Url) {
        url = v.toString();
    } else if (i == DownloadAttributes::BytesDownloaded) {
        bytesDownloaded = v.toLongLong();
    } else if (i == DownloadAttributes::Filename) {
        filename = v.toString();
    } else if (i == DownloadAttributes::Started) {
        started = v.toInt() == 1 ? true : false;
    } else if (i == DownloadAttributes::Status) {
        status = v.toString();
    } else if (i == DownloadAttributes::TransferRate) {
        transferRate = v.toString();
    } else if (i == DownloadAttributes::tempFileNames) {
        tempFileNames = v.toByteArray();
    } else if (i == DownloadAttributes::Filesize) {
        filesize = v.toLongLong();
    } else if (i == DownloadAttributes::ResumeCapability) {
        resumeCapability = v.toInt() == 1 ? true : false;
    }
}
