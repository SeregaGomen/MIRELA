#-------------------------------------------------
#
# Project created by QtCreator 2014-10-30T08:57:44
#
#-------------------------------------------------

QT       += core gui webkit webkitwidgets opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mpp
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    analyse.cpp \
    fldialog.cpp \
    glfun.cpp \
    glwidget.cpp \
    setupimagedialog.cpp

HEADERS  += mainwindow.h \
    analyse.h \
    matrix.h \
    fldialog.h \
    glfun.h \
    glwidget.h \
    imageparams.h \
    setupimagedialog.h

FORMS    += mainwindow.ui \
    funlistdialog.ui \
    setupimagedialog.ui

RESOURCES += \
    mpp.qrc
