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

#include "downloadview.h"
#include "enum.h"

#include <QHeaderView>
#include <QDesktopServices>
#include <QUrl>

DownloadView::DownloadView(QWidget* parent)
{

}

void DownloadView::loadViewSettings()
{
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setAlternatingRowColors(true);
    setSortingEnabled(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setTabKeyNavigation(true);
    setColumnHidden(0, true);
    setShowGrid(false);
    resizeColumnsToContents();
    verticalHeader()->hide();
    horizontalHeader()->setSectionHidden(Enum::DownloadAttributes::DatabaseId, true);
    horizontalHeader()->setSectionHidden(Enum::DownloadAttributes::Url, true);
    horizontalHeader()->setSectionHidden(Enum::DownloadAttributes::TempFileNames, true);
    horizontalHeader()->setSectionHidden(Enum::DownloadAttributes::Started, true);
    horizontalHeader()->setSectionsMovable(true);
    horizontalHeader()->setHighlightSections(false);
    horizontalHeader()->setDefaultAlignment(Qt::AlignJustify);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    sortByColumn(Enum::DownloadAttributes::RowId, Qt::SortOrder::AscendingOrder);
}
