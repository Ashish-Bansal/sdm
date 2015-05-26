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

#ifndef ENUM_H
#define ENUM_H

namespace DownloadAttributes {
    enum DownloadAttributes : int {
        Id = 0,
        Filename,
        Filesize,
        ResumeCapability,
        Url,
        BytesDownloaded,
        tempFileNames,
        DateAdded,
        Status,
        TransferRate,
        Started,
        END
    };
}

namespace SDM {
    enum Global : int {
        ResumeNotSupported,
        ResumeSupported,
        DownloadFinished,
        Failed,
        Successful
    };
}

namespace TableView {
    enum TableView : int {
        RowId = 0,
        DatabaseId,
        FileName,
        FileSize,
        DownloadProgress,
        TransferRate,
        Status,
        TimeRemaining,
        ResumeCapability,
        DateAdded,
        END
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
#endif // ENUM_H
