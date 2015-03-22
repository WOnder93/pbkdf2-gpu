TEMPLATE = app
CONFIG += console
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -l:libOpenCL.so.1

INCLUDEPATH += src

HEADERS += src/benchmark.h

SOURCES += src/main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libpbkdf2-compute-opencl/release/ -lpbkdf2-compute-opencl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libpbkdf2-compute-opencl/debug/ -lpbkdf2-compute-opencl
else:unix: LIBS += -L$$OUT_PWD/../libpbkdf2-compute-opencl/ -lpbkdf2-compute-opencl

INCLUDEPATH += $$PWD/../libpbkdf2-compute-opencl/include $$PWD/../libhashspec-hashalgorithm/include
DEPENDPATH += $$PWD/../libpbkdf2-compute-opencl

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpbkdf2-compute-opencl/release/libpbkdf2-compute-opencl.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpbkdf2-compute-opencl/debug/libpbkdf2-compute-opencl.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpbkdf2-compute-opencl/release/pbkdf2-compute-opencl.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpbkdf2-compute-opencl/debug/pbkdf2-compute-opencl.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libpbkdf2-compute-opencl/libpbkdf2-compute-opencl.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libpbkdf2-compute-cpu/release/ -lpbkdf2-compute-cpu
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libpbkdf2-compute-cpu/debug/ -lpbkdf2-compute-cpu
else:unix: LIBS += -L$$OUT_PWD/../libpbkdf2-compute-cpu/ -lpbkdf2-compute-cpu

INCLUDEPATH += $$PWD/../libpbkdf2-compute-cpu/include
DEPENDPATH += $$PWD/../libpbkdf2-compute-cpu

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpbkdf2-compute-cpu/release/libpbkdf2-compute-cpu.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpbkdf2-compute-cpu/debug/libpbkdf2-compute-cpu.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpbkdf2-compute-cpu/release/pbkdf2-compute-cpu.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpbkdf2-compute-cpu/debug/pbkdf2-compute-cpu.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libpbkdf2-compute-cpu/libpbkdf2-compute-cpu.a
