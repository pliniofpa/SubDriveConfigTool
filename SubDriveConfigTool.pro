#-------------------------------------------------
#
# Project created by QtCreator 2014-07-10T16:24:55
#
#-------------------------------------------------

QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SubDriveConfigTool
TEMPLATE = app

win32:RC_FILE = resources.rc

SOURCES += main.cpp\
        mainwindow.cpp \
    aboutdialog.cpp \
    networkconfigdialog.cpp \
    routersnetworkdialog.cpp

HEADERS  += mainwindow.h \
    aboutdialog.h \
    version.h \
    networkconfigdialog.h \
    build_version.h \
    routersnetworkdialog.h

win32:HEADERS +=

win32:INCLUDEPATH += "C:\Qt_Projects\SubDriveDataLogger"

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    networkconfigdialog.ui \
    dialog.ui \
    routersnetworkdialog.ui

CONFIG += mobility
MOBILITY = 

RESOURCES += \
    res.qrc

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android


