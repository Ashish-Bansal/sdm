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

#ifndef SINGLEINSTANCE_H
#define SINGLEINSTANCE_H

#include <QApplication>
#include <QLocalSocket>
#include <QLocalServer>

class SingleInstance : public QApplication
{
    Q_OBJECT
public:
    explicit SingleInstance(int argc, char *argv[]);
    ~SingleInstance();
    void stopServer();

private Q_SLOTS:
    void clientConnected();
    void socketDisconnected();

private:
    QLocalServer *mLocalServer;
    QLocalSocket *mLocalSocket;
    int mTimeToWait = -1;
    QString mServerName;

    void askForMainWindow();
};

#endif // SINGLEINSTANCE_H
