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

#ifndef DOWNLOADINFODIALOG_H
#define DOWNLOADINFODIALOG_H

#include "fetchheaders.h"
#include "databasemanager.h"
#include "singletonfactory.h"
#include <QDialog>

namespace Ui {
class DownloadInfoDialog;
}

class DownloadInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DownloadInfoDialog(QWidget *parent = 0);
    ~DownloadInfoDialog();
    void cancel();
    Ui::DownloadInfoDialog *ui;
};

#endif // DOWNLOADINFODIALOG_H
