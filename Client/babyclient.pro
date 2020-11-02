#-------------------------------------------------
#
# Project created by QtCreator 2020-10-30T09:18:59
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = babyclient
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    dialog.cpp

HEADERS  += widget.h \
    dialog.h \
    common.h

FORMS    += widget.ui \
    dialog.ui

RESOURCES += \
    img.qrc
