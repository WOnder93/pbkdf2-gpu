#-------------------------------------------------
#
# Project created by QtCreator 2015-04-02T13:16:46
#
#-------------------------------------------------

QT       -= core gui

TARGET = cipherspec-cipheralgorithm
TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11
CONFIG += create_prl

LIBS += -lcrypto

INCLUDEPATH += include/libcipherspec-cipheralgorithm src

SOURCES += \
    src/cipheralgorithm.cpp

HEADERS += \
    include/libcipherspec-cipheralgorithm/cipheralgorithm.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
