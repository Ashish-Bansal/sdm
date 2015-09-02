#ifndef MEMORYDATABASE_H
#define MEMORYDATABASE_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QSharedPointer>

#include "databasemanager.h"
#include "downloadproperties.h"
#include "downloadmodel.h"

class MemoryDatabase : public QObject
{
    Q_OBJECT
public:
    MemoryDatabase();
    ~MemoryDatabase();
    qint64 insertDownload(DownloadProperties properties);
    void removeDownload(qint64 id);
    void updateDetails(DownloadProperties properties);
    const DownloadProperties* getDetails(qint64 id);
    qint64 maxId();
    void readDatabase();
    int restartDownload(qint64 id);
    void setModel(QSharedPointer< DownloadModel > model);

private:
    QMap<int, DownloadProperties*> downloadList;
    DatabaseManager *mDbManager;
    QSharedPointer< DownloadModel > m_model;

public slots:
    void writeToDatabase();

signals:
    void downloadRemoved(qint64 id);
    void downloadInserted(qint64 id);
    void downloadFinished(qint64 id);
    void writeDatabase();
    void updateGUI(qint64 id);
    void downloadLoaded(qint64 id);
};

#endif // MEMORYDATABASE_H
