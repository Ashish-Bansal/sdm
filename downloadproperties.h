#ifndef DOWNLOADPROPERTIES_H
#define DOWNLOADPROPERTIES_H


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

QDebug operator<<(QDebug d, const DownloadProperties prop);

#endif // DOWNLOADPROPERTIES_H
