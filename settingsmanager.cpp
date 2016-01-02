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

#include "settingsmanager.h"
#include "enum.h"

#include <QApplication>
#include <QDebug>

SettingsManager::SettingsManager(QObject *parent) :
    QObject(parent),
    mFirstRun(false)
{
    QApplication::setOrganizationName("SDM");
    QApplication::setOrganizationDomain("ashish-bansal.in");
    QApplication::setApplicationName("SDM");
    mSettings = new QSettings();
    if (mSettings->status() != QSettings::NoError) {
        qDebug() << "Error opening settings file :" << mSettings->fileName();
        return;
    }

    // Application version has been used to check first run
    if (getValue(Enum::SettingType::General, QApplication::applicationVersion()) == QString()) {
        setValue(Enum::SettingType::General, QApplication::applicationVersion(), false);
        mFirstRun = true;
        firstRunDetected();
    }
}

bool SettingsManager::isFirstRun()
{
    return mFirstRun;
}

void SettingsManager::firstRunDetected()
{
    //ToDo: First run, Show changelog to user.
}

QVariant SettingsManager::getValue(int type, QString key)
{
    QVariant value;
    switch(type) {
        case Enum::SettingType::General :
            mSettings->beginGroup("General");
            value = mSettings->value(key);
            mSettings->endGroup();
            return value;
        default:
            return mSettings->value(key);
    };
}

void SettingsManager::setValue(int type, QString key, QVariant value)
{
    switch(type) {
        case Enum::SettingType::General :
            mSettings->beginGroup("General");
            mSettings->setValue(key, value);
            mSettings->endGroup();
            break;
        default:
            mSettings->setValue(key, value);
    };
    mSettings->sync();
}
