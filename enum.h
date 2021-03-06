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

#ifndef ENUM_H
#define ENUM_H

namespace Enum {
    namespace DownloadAttributes {
        enum DownloadAttributes : int {
            DatabaseId = 0,
            RowId,
            Filename,
            Filesize,
            ResumeCapability,
            Url,
            BytesDownloaded,
            TempFileNames,
            DateAdded,
            Status,
            TransferRate,
            TimeRemaining,
            DownloadProgress,
            Started,
            END
        };
    }

    namespace SDM {
        enum SDM : int {
            ResumeNotSupported,
            ResumeSupported,
            DownloadFinished,
            Failed,
            Successful
        };
    }

    namespace Status {
        enum Status : int {
            Idle,
            Completed,
            Downloading,
            Error,
            Merging
        };
    }

    namespace DownloadBackend {
        enum DownloadBackend : int {
            BytesDownloaded = 0,
            Status,
            TransferRate,
            END
        };
    }

    namespace SettingType {
        enum SettingType {
            General = 0,
            END
        };
    }
}

#endif // ENUM_H
