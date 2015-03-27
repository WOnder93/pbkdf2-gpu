#-------------------------------------------------
#
# Project created by QtCreator 2015-03-26T13:33:14
#
#-------------------------------------------------

QT       -= core gui

TARGET = commandline
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11
CONFIG += create_prl

INCLUDEPATH += include/libcommandline src

SOURCES += \
    src/argumenthandlers.cpp

HEADERS += \
    include/libcommandline/commandlineoption.h \
    include/libcommandline/commandlineparser.h \
    include/libcommandline/argumenthandlers.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
