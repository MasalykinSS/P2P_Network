#-------------------------------------------------
#
# Project created by QtCreator 2017-11-21T13:55:10
#
#-------------------------------------------------

QT       += core gui
CONFIG += C++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Client
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
LIBS +=-lws2_32

win32-g++ {
    INCLUDEPATH += D:/Boost/devenv/include
    LIBS += "-LD:/Boost/devenv/lib" \
            -llibboost_system \
            -llibboost_regex \
             -llibboost_filesystem \
} else:win32-msvc* {
    INCLUDEPATH += D:/Boost/devenv/include
    LIBS += "-LD:/Boost/devenv/lib" \
            -llibboost_system \
            -llibboost_regex \
             -llibboost_filesystem \
}

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    client_func.cpp \
    filesyst_func.cpp

HEADERS += \
        mainwindow.h \
    client_func.h \
    filesyst_func.h

FORMS += \
        mainwindow.ui
