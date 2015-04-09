#-------------------------------------------------
#
# Project created by QtCreator 2015-04-02T14:55:47
#
#-------------------------------------------------

QT       -= core gui

TARGET = ivmode
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11
CONFIG += create_prl

INCLUDEPATH += include/libivmode src

SOURCES += \
    src/ivgenerator.cpp \
    src/nullivgenerator.cpp \
    src/plainivgenerator.cpp \
    src/plain64ivgenerator.cpp \
    src/essivivgenerator.cpp

HEADERS += \
    include/libivmode/ivgenerator.h \
    src/nullivgenerator.h \
    src/plainivgenerator.h \
    src/plain64ivgenerator.h \
    src/essivivgenerator.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

# libcipherspec-cipheralgorithm:
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libcipherspec-cipheralgorithm/release/ -lcipherspec-cipheralgorithm
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libcipherspec-cipheralgorithm/debug/ -lcipherspec-cipheralgorithm
else:unix: LIBS += -L$$OUT_PWD/../libcipherspec-cipheralgorithm/ -lcipherspec-cipheralgorithm

INCLUDEPATH += $$PWD/../libcipherspec-cipheralgorithm/include
DEPENDPATH += $$PWD/../libcipherspec-cipheralgorithm/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libcipherspec-cipheralgorithm/release/libcipherspec-cipheralgorithm.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libcipherspec-cipheralgorithm/debug/libcipherspec-cipheralgorithm.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libcipherspec-cipheralgorithm/release/cipherspec-cipheralgorithm.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libcipherspec-cipheralgorithm/debug/cipherspec-cipheralgorithm.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libcipherspec-cipheralgorithm/libcipherspec-cipheralgorithm.a

# libhashspec-hashalgorithm:
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libhashspec-hashalgorithm/release/ -lhashspec-hashalgorithm
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libhashspec-hashalgorithm/debug/ -lhashspec-hashalgorithm
else:unix: LIBS += -L$$OUT_PWD/../libhashspec-hashalgorithm/ -lhashspec-hashalgorithm

INCLUDEPATH += $$PWD/../libhashspec-hashalgorithm/include
DEPENDPATH += $$PWD/../libhashspec-hashalgorithm/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libhashspec-hashalgorithm/release/libhashspec-hashalgorithm.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libhashspec-hashalgorithm/debug/libhashspec-hashalgorithm.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libhashspec-hashalgorithm/release/hashspec-hashalgorithm.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libhashspec-hashalgorithm/debug/hashspec-hashalgorithm.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libhashspec-hashalgorithm/libhashspec-hashalgorithm.a
