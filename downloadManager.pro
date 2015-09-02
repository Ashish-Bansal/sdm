#-------------------------------------------------
#
# Project created by QtCreator 2014-12-27T17:31:01
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets concurrent

TARGET = downloadManager
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    download.cpp \
    fetchheaders.cpp \
    adddialog.cpp \
    downloadinfodialog.cpp \
    databasemanager.cpp \
    localserver.cpp \
    startdownload.cpp \
    updater.cpp \
    singletonfactory.cpp \
    debug.cpp \
    global.cpp \
    memorydatabase.cpp \
    downloadproperties.cpp \
    downloadmodel.cpp

HEADERS += mainwindow.h \
    download.h \
    fetchheaders.h \
    adddialog.h \
    downloadinfodialog.h \
    databasemanager.h \
    localserver.h \
    startdownload.h \
    updater.h \
    singletonfactory.h \
    debug.h \
    enum.h \
    global.h \
    memorydatabase.h \
    downloadproperties.h \
    downloadmodel.h

FORMS += adddialog.ui \
    downloadinfodialog.ui

CONFIG += c++14
RESOURCES += icons.qrc
LIBS += -lcurl
