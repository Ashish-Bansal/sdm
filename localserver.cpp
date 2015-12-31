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

#include "localserver.h"
#include "downloadinfodialog.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

LocalServer::LocalServer(QObject *parent) :
    QObject(parent)
{
    mWebSocketServer = new QWebSocketServer(QStringLiteral("Echo Server"),
                                            QWebSocketServer::NonSecureMode);
    m_port = 33533;

}

LocalServer::~LocalServer()
{
    stopServer();
}

void LocalServer::startListening()
{
    if (mWebSocketServer->listen(QHostAddress::Any, m_port)) {

        qDebug() << "Local Server listening on port :" << m_port;
        connect(mWebSocketServer, &QWebSocketServer::newConnection,
                this, &LocalServer::clientConnected);
    }
}

void LocalServer::stopServer()
{
    mWebSocketServer->close();
    qDeleteAll(mClients.begin(), mClients.end());
}

void LocalServer::clientConnected()
{
    qDebug() << "Client Connected";
    QWebSocket *socket = mWebSocketServer->nextPendingConnection();

    connect(socket, &QWebSocket::textMessageReceived, this, &LocalServer::processTextMessage);
    connect(socket, &QWebSocket::disconnected, this, &LocalServer::socketDisconnected);

    mClients << socket;
}

void LocalServer::processTextMessage(QString data)
{
    QWebSocket *socket = qobject_cast<QWebSocket *>(sender());

    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
    QString url = doc.object().take("url").toString();
    if (url != "") {
        emit downloadRequested(url);
    }
}

void LocalServer::socketDisconnected()
{
    QWebSocket *socket = qobject_cast<QWebSocket *>(sender());

    qDebug() << "socketDisconnected:" << socket;
    if (socket) {
        mClients.removeAll(socket);
        socket->deleteLater();
    }
}
