#ifndef DOWNLOADATTRIBUTES_H
#define DOWNLOADATTRIBUTES_H

#include "enum.h"

#include <QObject>
#include <QString>

class DownloadAttributes : public QObject
{
    Q_OBJECT
public:
    DownloadAttributes();
    DownloadAttributes(const DownloadAttributes &properties);
    DownloadAttributes(const DownloadAttributes *properties);
    DownloadAttributes& operator=(const DownloadAttributes&);
    void setValue(int i, QVariant v);

    int id;
    QString filename;
    qint64 filesize;
    int resumeCapability;
    QString url;
    qint64 bytesDownloaded;
    QByteArray tempFileNames;
    QString dateAdded;
    int status;
    QString transferRate;
    bool started;
};

QDebug operator<<(QDebug d, const DownloadAttributes prop);

#endif // DOWNLOADATTRIBUTES_H
