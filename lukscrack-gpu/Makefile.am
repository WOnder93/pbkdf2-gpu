AM_CXXFLAGS = -I$(srcdir)/src

bin_PROGRAMS = lukscrack-gpu

AM_CXXFLAGS += -I$(srcdir)/../libhashspec-hashalgorithm/include
AM_CXXFLAGS += -I$(srcdir)/../libcipherspec-cipheralgorithm/include
AM_CXXFLAGS += -I$(srcdir)/../libivmode/include
AM_CXXFLAGS += -I$(srcdir)/../libcommandline/include
AM_CXXFLAGS += -I$(srcdir)/../libpbkdf2-compute-opencl/include

lukscrack_gpu_LDADD = -l$(OPENCL_LIB) -lpthread \
    ../libhashspec-hashalgorithm/libhashspec-hashalgorithm.la \
    ../libcipherspec-cipheralgorithm/libcipherspec-cipheralgorithm.la \
    ../libivmode/libivmode.la \
    ../libcommandline/libcommandline.la \
    ../libpbkdf2-compute-opencl/libpbkdf2-compute-opencl.la

lukscrack_gpu_SOURCES = src/main.cpp \
    src/lukscrack.cpp \
    src/passworddata.cpp \
    src/passwordgenerator.cpp \
    src/listpasswordgenerator.cpp \
    src/sectordecryptor.cpp \
    src/crypto/af.cpp \
    src/gpu/batchprocessingcontext.cpp \
    src/gpu/crackingcontext.cpp \
    src/gpu/devicecrackingcontext.cpp \
    src/lukscrack.h \
    src/passworddata.h \
    src/passwordgenerator.h \
    src/listpasswordgenerator.h \
    src/passworddistributor.h \
    src/sectordecryptor.h \
    src/crypto/af.h \
    src/gpu/batchprocessingcontext.h \
    src/gpu/crackingcontext.h \
    src/gpu/devicecrackingcontext.h

DATA_PATH = $(srcdir)/../libpbkdf2-compute-opencl

all-local:
	rm -rf data && $(LN_S) $(DATA_PATH)/data data

clean-local:
	rm -rf data
