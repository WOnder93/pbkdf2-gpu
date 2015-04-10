#-------------------------------------------------
#
# Project created by QtCreator 2015-04-10T10:37:05
#
#-------------------------------------------------

QT       -= core gui

TARGET = pbkdf2-gpu-common
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11
CONFIG += create_prl

INCLUDEPATH += include/libpbkdf2-gpu-common src

SOURCES +=

HEADERS += \
    include/libpbkdf2-gpu-common/alignment.h \
    include/libpbkdf2-gpu-common/endianness.h \
    include/libpbkdf2-gpu-common/logging.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
