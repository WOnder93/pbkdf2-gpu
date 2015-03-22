#-------------------------------------------------
#
# Project created by QtCreator 2015-03-17T20:03:59
#
#-------------------------------------------------

QT       -= core gui

TARGET = hashspec-openssl
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11

LIBS += -lcrypto

INCLUDEPATH += include/libhashspec-openssl src

SOURCES += \
	src/digestlookup.cpp

HEADERS += \
	include/libhashspec-openssl/digestlookup.h

unix {
	target.path = /usr/lib
	INSTALLS += target
}
