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

#include "adddialog.h"
#include "ui_adddialog.h"
#include "fetchheaders.h"
#include "downloadinfodialog.h"
#include "singletonfactory.h"
#include "global.h"

#include <QDebug>
#include <QMessageBox>

AddDialog::AddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddDialog)
{
    ui->setupUi(this);
    ui->label_2->setVisible(false);
//    ui->pushButtonOk->setEnabled(false);
    ui->lineEdit->setText("http://upload.wikimedia.org/wikipedia/commons/2/2d/Snake_River_%285mb%29.jpg");
    connect(ui->lineEdit, &QLineEdit::textChanged, this, &AddDialog::validateUrl);
    connect(ui->pushButtonOk, &QPushButton::clicked, this, &AddDialog::urlAccepted);
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &QDialog::close);
}

AddDialog::~AddDialog()
{
    delete ui;
}

void AddDialog::urlAccepted()
{
    QString url = ui->lineEdit->text();
    this->close();
    emit showDownloadDialog(url);
}

void AddDialog::validateUrl()
{
    QString url = ui->lineEdit->text();

    if (url == "" ||  SDM::validateUrl(url)) {
        ui->label_2->setEnabled(true);
        ui->label_2->setVisible(true);
        ui->pushButtonOk->setEnabled(false);
    } else {
        ui->pushButtonOk->setEnabled(true);
        ui->label_2->setVisible(false);
    }
}
