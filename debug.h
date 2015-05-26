#include <iostream>
#include <sstream>
#include <QObject>
#include <QDebug>

#include <stdio.h>
#include <stdlib.h>
class Debug {

public:
    Debug();
    static void debugStyle(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};
