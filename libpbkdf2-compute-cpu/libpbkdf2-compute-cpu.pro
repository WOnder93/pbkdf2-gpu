#-------------------------------------------------
#
# Project created by QtCreator 2015-03-14T11:45:53
#
#-------------------------------------------------

QT       -= core gui

TARGET = pbkdf2-compute-cpu
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11
CONFIG += create_prl

LIBS += -lcrypto

INCLUDEPATH += include/libpbkdf2-compute-cpu src

SOURCES += \
    src/hashfunctioncontext.cpp \
    src/processingunit.cpp

HEADERS += \
    include/libpbkdf2-compute-cpu/hashfunctioncontext.h \
    include/libpbkdf2-compute-cpu/computecontext.h \
    include/libpbkdf2-compute-cpu/devicecontext.h \
    include/libpbkdf2-compute-cpu/processingunit.h \
    include/libpbkdf2-compute-cpu/types.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

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
