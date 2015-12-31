/*
 * Copyright 2015 Ashish Bansal<bansal.ashish096@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "singleinstance.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

SingleInstance::SingleInstance(int argc, char *argv[]) :
    QApplication(argc, argv),
    mServerName("sdm")
{
    mLocalSocket = new QLocalSocket();

    // Use of mServerName crashes the application;
    mLocalSocket->connectToServer("sdm");
    if (mLocalSocket->waitForConnected(mTimeToWait)) {
        askForMainWindow();
        ::exit(0);
    }

    mLocalServer = new QLocalServer();
    mLocalServer->setSocketOptions(QLocalServer::UserAccessOption);
    connect(mLocalServer, &QLocalServer::newConnection, this, &SingleInstance::clientConnected);

    // Use of mServerName crashes the application;
    mLocalServer->listen("sdm");
}

SingleInstance::~SingleInstance()
{
    stopServer();
}

void SingleInstance::stopServer()
{
    mLocalServer->close();
}

void SingleInstance::clientConnected()
{
    qDebug() << "Client Connected";
    emit showMainWindow();

    QLocalSocket *socket = mLocalServer->nextPendingConnection();

    connect(socket, &QLocalSocket::disconnected, this, &SingleInstance::socketDisconnected);
}

void SingleInstance::askForMainWindow()
{
    qDebug() << "Instance already created, opening main window";
    mLocalSocket->write("askForMainWindow");
}

void SingleInstance::socketDisconnected()
{
    QLocalSocket *socket = qobject_cast<QLocalSocket *>(sender());

    Q_ASSERT(socket != nullptr);

    socket->deleteLater();
}
