#-------------------------------------------------
#
# Project created by QtCreator 2016-09-07T15:44:21
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TransferFileInfoByNet
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    filemodel.cpp \
    server.cpp \
    client.cpp \
    jsonhelper.cpp \
    driver.cpp \
    delegate.cpp

HEADERS  += widget.h \
    filemodel.h \
    server.h \
    client.h \
    jsonhelper.h \
    driver.h \
    delegate.h

FORMS    += widget.ui
