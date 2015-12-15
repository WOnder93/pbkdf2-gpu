#-------------------------------------------------
#
# Project created by QtCreator 2015-04-02T13:16:46
#
#-------------------------------------------------

TARGET = cipherspec-cipheralgorithm
TEMPLATE = lib
CONFIG -= qt
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
