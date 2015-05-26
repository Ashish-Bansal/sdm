#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>

namespace SDM
{
    bool validateUrl(QString url);
    QString convertUnits(qint64 bytes);
}

#endif // GLOBAL_H
