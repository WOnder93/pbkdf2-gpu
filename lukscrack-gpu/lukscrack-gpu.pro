TEMPLATE = app
CONFIG += console
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += link_prl

INCLUDEPATH += src

LIBS += -l:libOpenCL.so.1 -lpthread

SOURCES += src/main.cpp \
    src/lukscrack.cpp \
    src/passworddata.cpp \
    src/passwordgenerator.cpp \
    src/listpasswordgenerator.cpp \
    src/sectordecryptor.cpp \
    src/afmerger.cpp \
    src/threadpool.cpp \
    src/gpu/batchprocessingcontext.cpp \
    src/gpu/crackingcontext.cpp \
    src/gpu/devicecrackingcontext.cpp

HEADERS += \
    src/lukscrack.h \
    src/passworddata.h \
    src/passwordgenerator.h \
    src/listpasswordgenerator.h \
    src/passworddistributor.h \
    src/sectordecryptor.h \
    src/afmerger.h \
    src/threadpool.h \
    src/gpu/batchprocessingcontext.h \
    src/gpu/crackingcontext.h \
    src/gpu/devicecrackingcontext.h

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

# libivmode:
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libivmode/release/ -livmode
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libivmode/debug/ -livmode
else:unix: LIBS += -L$$OUT_PWD/../libivmode/ -livmode

INCLUDEPATH += $$PWD/../libivmode/include
DEPENDPATH += $$PWD/../libivmode/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libivmode/release/libivmode.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libivmode/debug/libivmode.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libivmode/release/ivmode.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libivmode/debug/ivmode.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libivmode/libivmode.a

# libcommandline:
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libcommandline/release/ -lcommandline
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libcommandline/debug/ -lcommandline
else:unix: LIBS += -L$$OUT_PWD/../libcommandline/ -lcommandline

INCLUDEPATH += $$PWD/../libcommandline/include
DEPENDPATH += $$PWD/../libcommandline/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libcommandline/release/libcommandline.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libcommandline/debug/libcommandline.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libcommandline/release/commandline.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libcommandline/debug/commandline.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libcommandline/libcommandline.a

# libpbkdf2-compute-opencl:
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libpbkdf2-compute-opencl/release/ -lpbkdf2-compute-opencl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libpbkdf2-compute-opencl/debug/ -lpbkdf2-compute-opencl
else:unix: LIBS += -L$$OUT_PWD/../libpbkdf2-compute-opencl/ -lpbkdf2-compute-opencl

INCLUDEPATH += $$PWD/../libpbkdf2-compute-opencl/include  $$PWD/../libhashspec-hashalgorithm/include
DEPENDPATH += $$PWD/../libpbkdf2-compute-opencl/include  $$PWD/../libhashspec-hashalgorithm/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpbkdf2-compute-opencl/release/libpbkdf2-compute-opencl.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpbkdf2-compute-opencl/debug/libpbkdf2-compute-opencl.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpbkdf2-compute-opencl/release/pbkdf2-compute-opencl.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpbkdf2-compute-opencl/debug/pbkdf2-compute-opencl.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libpbkdf2-compute-opencl/libpbkdf2-compute-opencl.a
