/***********************************************************************************
 *   Copyright 2015 Ashish Bansal<bansal.ashish096@gmail.com>                      *
 *                                                                                 *
 *                                                                                 *
 *   This library is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public                    *
 *   License as published by the Free Software Foundation; either                  *
 *   version 2.1 of the License, or (at your option) any later version.            *
 *                                                                                 *
 *   This library is distributed in the hope that it will be useful,               *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of                *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
 *   Lesser General Public License for more details.                               *
 *                                                                                 *
 *   You should have received a copy of the GNU Lesser General Public              *
 *   License along with this library.  If not, see <http://www.gnu.org/licenses/>. *
 ***********************************************************************************/

#ifndef LOCALSERVER_H
#define LOCALSERVER_H

#include <QTcpSocket>
#include <QTcpServer>
#include <QObject>
class LocalServer : public QObject
{
    Q_OBJECT
public:
    LocalServer();
    ~LocalServer();
    QTcpSocket *socket;
    QTcpServer *tcpServer;
    QTcpSocket *clientConnection;

public slots :
    void connectToHost();
    void readClient();
};

#endif // LOCALSERVER_H
