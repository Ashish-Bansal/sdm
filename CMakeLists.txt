project(downloadManager)
cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} -Wall -std=c++14")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Declarative REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Concurrent REQUIRED)
set (downloadManager_SRCS
        main.cpp
        mainwindow.cpp
        download.cpp
        fetchheaders.cpp
        adddialog.cpp
        downloadinfodialog.cpp
        databasemanager.cpp
        localserver.cpp
        startdownload.cpp
        singletonfactory.cpp
        updater.cpp
        debug.cpp
        global.cpp
        downloadattributes.cpp
        downloadmodel.cpp
        downloadview.cpp
        proxymodel.cpp
        singleinstance.cpp
        settingsmanager.cpp
    )
QT5_WRAP_UI(UIS adddialog.ui downloadinfodialog.ui)
QT5_ADD_RESOURCES(RSCS icons.qrc)

add_executable(downloadManager ${downloadManager_SRCS} ${UIS} ${RSCS})
target_link_libraries(downloadManager ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTSQL_LIBRARY})

qt5_use_modules(downloadManager Widgets Network Sql Concurrent WebSockets)
