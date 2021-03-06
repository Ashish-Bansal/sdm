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

#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include "headerlist.h"

#include <QObject>
#include <QWebSocket>
#include <QWebSocketServer>
#include <QList>

class WebSocketServer : public QObject
{
    Q_OBJECT
public:
    explicit WebSocketServer(QObject *parent = 0);
    ~WebSocketServer();
    void startListening();
    void stopServer();

signals:
    void downloadRequested(QString url, HeaderList headerList);

private Q_SLOTS:
    void clientConnected();
    void processTextMessage(QString data);
    void socketDisconnected();

private:
    int m_port;
    QWebSocketServer *mWebSocketServer;
    QList<QWebSocket *> mClients;
};

#endif // WEBSOCKETSERVER_H
