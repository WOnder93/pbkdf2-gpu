AM_CXXFLAGS = -I$(srcdir)/src

bin_PROGRAMS = pbkdf2-compute-tests

AM_CXXFLAGS += -I$(srcdir)/../libpbkdf2-compute-opencl/include -I$(srcdir)/../libhashspec-hashalgorithm/include
AM_CXXFLAGS += -I$(srcdir)/../libpbkdf2-compute-cpu/include

pbkdf2_compute_tests_LDADD = ../libpbkdf2-compute-opencl/libpbkdf2-compute-opencl.la ../libpbkdf2-compute-cpu/libpbkdf2-compute-cpu.la -l$(OPENCL_LIB)
pbkdf2_compute_tests_SOURCES = src/main.cpp \
    src/pbkdf2testvector.cpp \
    src/testsuite.cpp \
    src/tests.cpp \
    src/testexception.h \
    src/test.h \
    src/pbkdf2testvector.h \
    src/utils.h \
    src/testsuite.h \
    src/tests.h

DATA_PATH = $(srcdir)/../libpbkdf2-compute-opencl

all-local:
	rm -rf data && $(LN_S) $(DATA_PATH)/data data

clean-local:
	rm -rf data
