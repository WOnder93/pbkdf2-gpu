#-------------------------------------------------
#
# Project created by QtCreator 2015-03-19T12:30:41
#
#-------------------------------------------------

QT       -= core gui

TARGET = hashspec-hashalgorithm
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11
CONFIG += create_prl

LIBS += -lcrypto

INCLUDEPATH += include/libhashspec-hashalgorithm src

SOURCES += \
    src/hashalgorithm.cpp

HEADERS += \
    include/libhashspec-hashalgorithm/hashalgorithm.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

# libhashspec-openssl:
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libhashspec-openssl/release/ -lhashspec-openssl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libhashspec-openssl/debug/ -lhashspec-openssl
else:unix: LIBS += -L$$OUT_PWD/../libhashspec-openssl/ -lhashspec-openssl

INCLUDEPATH += $$PWD/../libhashspec-openssl/include
DEPENDPATH += $$PWD/../libhashspec-openssl/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libhashspec-openssl/release/libhashspec-openssl.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libhashspec-openssl/debug/libhashspec-openssl.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libhashspec-openssl/release/hashspec-openssl.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libhashspec-openssl/debug/hashspec-openssl.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libhashspec-openssl/libhashspec-openssl.a
