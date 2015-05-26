#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QByteArray>
#include <QMultiMap>
#include <QVariant>

namespace SDM
{
    bool validateUrl(QString url);
    QString convertUnits(qint64 bytes);
    QMultiMap<double, QMultiMap<qint8, QVariant> > readByteArray(QByteArray b);
    QByteArray writeToByteArray(QMultiMap <double, QMultiMap <qint8, QVariant> > m);
    QString filenameFromUrl(QString url);
    QString filenameFromUrl(QUrl url);
}

#endif // GLOBAL_H
