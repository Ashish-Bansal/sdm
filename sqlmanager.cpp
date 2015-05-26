#include "sqlmanager.h"

#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>

sqlManager* sqlManager::sSqlManager = NULL;

sqlManager* sqlManager::instance()
{
    if(sSqlManager == NULL){
        sSqlManager = new sqlManager();
    }
    return sSqlManager;
}

sqlManager::sqlManager()
{
    mydb = QSqlDatabase::addDatabase("QSQLITE");
    mydb.setDatabaseName("/tmp/downloadList");

    if(!mydb.open()){
        qDebug() << "Unable to open database";
        return;
    }else{
        qDebug() << "Database opened successfully";
    }

    QSqlQuery qry;
    QString qryStr = "CREATE TABLE IF NOT EXISTS downloader(FILENAME varchar(255), SIZE varchar(10), RESUME int);";

    if(!qry.exec(qryStr)){
        qDebug() << "Unable to create table!";
    }else{
        qDebug() << "Table successfully created";
    }
}

sqlManager::~sqlManager()
{
    qDebug() << "SQL Manager Destructor Called";
}

void sqlManager::insertRow(QString filename, QString filesize, int resumeCapability)
{
    QSqlQuery qry;
    QString qryStr = "INSERT INTO downloader VALUES (" + filename + "," + filesize + "," + resumeCapability + ");" ;
    if(!qry.exec(qryStr)){
        qDebug() << "Unable to insert row in table!";
    }else{
        qDebug() << "Row successfully created";
    }
}
