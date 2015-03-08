#-------------------------------------------------
#
# Project created by QtCreator 2015-03-05T18:29:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    opendialog.cpp

HEADERS  += mainwindow.h \
    openfiledialog.h \
    opendialog.h

FORMS    += \
    mainwindow.ui \
    opendialog.ui
