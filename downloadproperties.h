#ifndef DOWNLOADPROPERTIES_H
#define DOWNLOADPROPERTIES_H

#include "enum.h"

#include <QObject>
#include <QString>

class DownloadProperties : public QObject
{
    Q_OBJECT
public:
    DownloadProperties();
    DownloadProperties(const DownloadProperties &properties);
    DownloadProperties(const DownloadProperties *properties);
    DownloadProperties& operator=(const DownloadProperties&);
    void setValue(int i, QVariant v);

    int id = 0;
    QString filename = QString();
    qint64 filesize = 0;
    int resumeCapability = SDM::ResumeNotSupported;
    QString url = QString();
    qint64 bytesDownloaded = 0;
    QByteArray tempFileNames;
    QString dateAdded = QString();
    int status = Status::Idle;
    QString transferRate = QString();
    bool started = false;

};

QDebug operator<<(QDebug d, const DownloadProperties prop);

#endif // DOWNLOADPROPERTIES_H
