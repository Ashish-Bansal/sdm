#include "memorydatabase.h"
#include "enum.h"
#include "global.h"
#include "singletonfactory.h"
#include "startdownload.h"

#include <QTimer>
#include <QHash>
#include <QVariant>
#include <QDebug>
#include <QByteArray>
#include <QtConcurrent>
#include <QThreadPool>

MemoryDatabase::MemoryDatabase()
{
    mDbManager = new DatabaseManager();
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MemoryDatabase::writeToDatabase);
    timer->start(10000);

    connect(this, &MemoryDatabase::downloadInserted, this, &MemoryDatabase::writeToDatabase);
    connect(this, &MemoryDatabase::downloadRemoved, this, &MemoryDatabase::writeToDatabase);
}

qint64 MemoryDatabase::insertDownload(DownloadProperties properties)
{
    qint64 id = maxId() + 1;
    if (downloadList.value(id, nullptr) == nullptr) {
        qDebug() << "ID duplication found, returning.";
    }
    DownloadProperties *dld = new DownloadProperties(properties);
    downloadList.insert(id, dld);
    dld->id = id;

    emit downloadInserted(id);
    QtConcurrent::run(mDbManager, &DatabaseManager::insertDownload,
                      DownloadProperties(getDetails(id)));
    emit updateGUI(id);
    return id;
}

qint64 MemoryDatabase::maxId()
{
    auto it = downloadList.begin();
    qint64 max = 0;
    for(; it != downloadList.end(); it++) {
        if (it.key() > max) {
            max = it.key();
        }
    }
    return max;
}

void MemoryDatabase::updateDetails(const DownloadProperties properties)
{
    qint64 id = properties.id;
    DownloadProperties *dld = downloadList.value(id, nullptr);
    if (dld == nullptr) {
        return;
    }
    dld->id = id;
    dld->filename = properties.filename;
    dld->filesize = properties.filesize;
    dld->resumeCapability = properties.resumeCapability;
    dld->url = properties.url;
    dld->bytesDownloaded = properties.bytesDownloaded;
    dld->transferRate = properties.transferRate;
    dld->status = properties.status;
    dld->tempFileNames = properties.tempFileNames;
    dld->started = properties.started;

    emit updateGUI(id);
}

const DownloadProperties* MemoryDatabase::getDetails(qint64 id)
{
    auto val = downloadList.value(id, nullptr);
    if (val == nullptr) {
        qDebug() << "not found";
    }
    return val;
}

void MemoryDatabase::removeDownload(qint64 id)
{
    if (downloadList.remove(id) == 0){
        qDebug() << "Id not stored in database";
    }

    QtConcurrent::run(mDbManager, &DatabaseManager::removeDownload, id);
    emit downloadRemoved(id);
}

int MemoryDatabase::restartDownload(qint64 id)
{
    DownloadProperties *prop = downloadList.value(id, nullptr);
    if (prop == nullptr) {
        qDebug() << "No Id found";
        return SDM::Failed;
    }

    StartDownload download(id);
    download.cleanUp();

    prop->bytesDownloaded = 0;
    prop->started = false;
    prop->transferRate = QString();
    prop->tempFileNames;
    QtConcurrent::run(mDbManager, &DatabaseManager::updateDetails,
                      DownloadProperties(getDetails(id)));
    emit updateGUI(id);
    return SDM::Successful;
}

void MemoryDatabase::writeToDatabase()
{
    auto it = downloadList.begin();
    for(; it != downloadList.end(); it++) {
            QtConcurrent::run(mDbManager, &DatabaseManager::updateDetails,
                              DownloadProperties(getDetails(it.key())));
    }
}

void MemoryDatabase::readDatabase()
{
    QString qryStr = "SELECT * from downloadList;";
    QSharedPointer<QSqlQuery> it  = mDbManager->getIterator(qryStr);

    if (it.isNull()) {
        return;
    }

    while (it->next()) {
        DownloadProperties *dld = new DownloadProperties();
        for(int i = 0; i < DownloadAttributes::END; i++){
            dld->setValue(i, it->value(i));
        }
        downloadList.insert(dld->id, dld);
        emit downloadLoaded(it->value(0).toLongLong());
    }
}
