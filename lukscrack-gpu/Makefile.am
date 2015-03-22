AM_CXXFLAGS = -I$(srcdir)/src

bin_PROGRAMS = lukscrack-gpu

AM_CXXFLAGS += -I$(srcdir)/../libpbkdf2-compute-opencl/include -I$(srcdir)/../libhashspec-hashalgorithm/include

lukscrack_gpu_LDADD = ../libpbkdf2-compute-opencl/libpbkdf2-compute-opencl.la -l:libOpenCL.so.1 -lcrypto
lukscrack_gpu_SOURCES = src/main.cpp \
    src/lukscrack.cpp \
    src/passworddata.cpp \
    src/cipheralgorithm.cpp \
    src/crypto/af.cpp \
    src/crypto/crypto_backend_openssl.cpp \
    src/crypto/crypto_backend_linux.cpp \
    src/gpu/batchprocessingcontext.cpp \
    src/gpu/crackingcontext.cpp \
    src/lukscrack.h \
    src/passwordgenerator.h \
    src/passworddata.h \
    src/cipheralgorithm.h \
    src/crypto/af.h \
    src/crypto/crypto_backend.h \
    src/crypto/errno.h \
    src/gpu/batchprocessingcontext.h \
    src/gpu/crackingcontext.h

DATA_PATH = $(srcdir)/../libpbkdf2-compute-opencl

all-local:
	rm -rf data && $(LN_S) $(DATA_PATH)/data data

clean-local:
	rm -rf data
