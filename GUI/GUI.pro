#-------------------------------------------------
#
# Project created by QtCreator 2015-03-05T18:29:35
#
#-------------------------------------------------

QT       += core gui

CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    opendialog.cpp \
    cgp_functions.c \
    cgp_functions_warehouse.c \
    cgpsettings.cpp

HEADERS  += mainwindow.h \
    openfiledialog.h \
    opendialog.h \
    cgp.h \
    cgp_warehouse.h \
    cgpsettings.h \
    globals.h


FORMS    += \
    mainwindow.ui \
    opendialog.ui \
    cgpsettings.ui
