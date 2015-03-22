TEMPLATE = app
CONFIG += console
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += src

LIBS += -l:libOpenCL.so.1 -lcrypto

SOURCES += src/main.cpp \
	src/lukscrack.cpp \
	src/passworddata.cpp \
	src/cipheralgorithm.cpp \
	src/crypto/af.cpp \
	src/crypto/crypto_backend_openssl.cpp \
	src/crypto/crypto_backend_linux.cpp \
	src/gpu/batchprocessingcontext.cpp \
	src/gpu/crackingcontext.cpp

HEADERS += \
	src/lukscrack.h \
	src/passwordgenerator.h \
	src/passworddata.h \
	src/cipheralgorithm.h \
	src/crypto/af.h \
	src/crypto/crypto_backend.h \
	src/crypto/errno.h \
	src/gpu/batchprocessingcontext.h \
	src/gpu/crackingcontext.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libpbkdf2-compute-opencl/release/ -lpbkdf2-compute-opencl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libpbkdf2-compute-opencl/debug/ -lpbkdf2-compute-opencl
else:unix: LIBS += -L$$OUT_PWD/../libpbkdf2-compute-opencl/ -lpbkdf2-compute-opencl

INCLUDEPATH += $$PWD/../libpbkdf2-compute-opencl/include  $$PWD/../libhashspec-hashalgorithm/include
DEPENDPATH += $$PWD/../libpbkdf2-compute-opencl

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpbkdf2-compute-opencl/release/libpbkdf2-compute-opencl.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpbkdf2-compute-opencl/debug/libpbkdf2-compute-opencl.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpbkdf2-compute-opencl/release/pbkdf2-compute-opencl.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpbkdf2-compute-opencl/debug/pbkdf2-compute-opencl.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libpbkdf2-compute-opencl/libpbkdf2-compute-opencl.a
