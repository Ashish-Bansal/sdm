#ifndef SQLMANAGER_H
#define SQLMANAGER_H

#include <QString>
#include <QObject>
#include <QSqlDatabase>
class sqlManager :public QObject
{
    Q_OBJECT

public:
    static sqlManager* instance();
    static void insertRow(QString filename, QString filesize, int resumeCapability);

    QSqlDatabase mydb;

private:
    sqlManager();
    ~sqlManager();
    static sqlManager *sSqlManager;


signals:
    void downloadAdded();
};

#endif // SQLMANAGER_H
