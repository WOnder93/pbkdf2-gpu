#-------------------------------------------------
#
# Project created by QtCreator 2015-03-03T19:14:22
#
#-------------------------------------------------

QT       -= core gui

TARGET = pbkdf2-compute-opencl
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11
CONFIG += create_prl

LIBS += -l:libOpenCL.so.1

INCLUDEPATH += include/libpbkdf2-compute-opencl src

HEADERS += \
    include/libpbkdf2-compute-opencl/cl.hpp \
    include/libpbkdf2-compute-opencl/opencl.h \
    include/libpbkdf2-compute-opencl/hashfunctioncontext.h \
    include/libpbkdf2-compute-opencl/computecontext.h \
    include/libpbkdf2-compute-opencl/devicecontext.h \
    include/libpbkdf2-compute-opencl/processingunit.h \
    include/libpbkdf2-compute-opencl/globalcontext.h \
    include/libpbkdf2-compute-opencl/types.h \
    src/kernelloader.h \
    include/libpbkdf2-compute-opencl/device.h

SOURCES += \
    src/computecontext.cpp \
    src/kernelloader.cpp \
    src/devicecontext.cpp \
    src/hashfunctioncontext.cpp \
    src/processingunit.cpp \
    src/device.cpp \
    src/globalcontext.cpp

OTHER_FILES += \
    data/opencl/kernels/pbkdf2_sha1.cl \
    data/opencl/kernels/pbkdf2_sha256.cl \
    data/opencl/kernels/pbkdf2_generic.cl \
    data/opencl/kernels/sha_common.cl \
    data/opencl/kernels/hash_sha1.cl \
    data/opencl/kernels/hash_sha256.cl \
    data/opencl/kernels/hash_ripemd160.cl \
    data/opencl/kernels/pbkdf2_ripemd160.cl \
    data/opencl/kernels/hash_word_uint.cl

unix {
    target.path = /usr/lib
    INSTALLS += target
}

# libpbkdf2-gpu-common:
INCLUDEPATH += $$PWD/../libpbkdf2-gpu-common/include
DEPENDPATH += $$PWD/../libpbkdf2-gpu-common/include

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
