#ifndef MEMORYDATABASE_H
#define MEMORYDATABASE_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QSharedPointer>

#include "databasemanager.h"
#include "downloadattributes.h"
#include "downloadmodel.h"

class MemoryDatabase : public QObject
{
    Q_OBJECT
public:
    MemoryDatabase();
    ~MemoryDatabase();
    qint64 insertDownload(DownloadAttributes properties);
    void removeDownload(qint64 id);
    void updateDetails(DownloadAttributes properties);
    const DownloadAttributes* getDetails(qint64 id);
    qint64 maxId();
    void readDatabase();
    int restartDownload(qint64 id);
    void setModel(QSharedPointer<DownloadModel> model);

private:
    QMap<int, DownloadAttributes*> m_downloadList;
    DatabaseManager *m_dbManager;
    QSharedPointer<DownloadModel> m_model;

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
