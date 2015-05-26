#include "downloadaswhole.h"

DownloadAsWhole::DownloadAsWhole(QString )
{
}

void DownloadAsWhole::startDownloadAsWhole()
{
    qDebug() << "Downloading as whole";
    delete(qnam);
    qnam = new QNetworkAccessManager();
    req->setRawHeader("Range", "");
    downloadReply = qnam->get(*req);
    connect(downloadReply, &QNetworkReply::downloadProgress, this, &Download::downloadingAsWholeProgress);
    connect(qnam, &QNetworkAccessManager::finished, this, &Download::finishedDownloadingAsWhole);

    downloadTimer.start();
    timeGap.start();
    previousDownloadSize = 0;
}

void DownloadAsWhole::downloadingAsWholeProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if(timeGap.elapsed() < 200){
        return;
    }
    qDebug() << "Downloading Speed :" << (bytesReceived-previousDownloadSize)/downloadTimer.restart() << "KB/s";
    previousDownloadSize = bytesReceived;
    QString units[5] = {"B", "KB", "MB", "GB", "TB"};
    if(bytesReceived == 0){
        qDebug() << "No bytes recieved";
        return;
    }
    QDataStream out(&file);
    QByteArray data =  downloadReply->readAll();
    out.writeRawData(data, data.length());
    int pow=floor(log((double)bytesReceived)/log(1024));
    pow = qMin(pow, 5);
    qDebug()<< "Bytes Downloaded : " << bytesReceived/qPow(1024, pow) << units[pow];
    timeGap.restart();
}

void DownloadAsWhole::finishedDownloadingAsWhole(QNetworkReply* reply)
{
    qDebug() << "Download Finished as whole file";
    file.close();
    reply->deleteLater();
}
