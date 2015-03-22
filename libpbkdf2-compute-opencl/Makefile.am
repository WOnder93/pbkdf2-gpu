AM_CXXFLAGS = -I$(srcdir)/src -I$(srcdir)/include/libpbkdf2-compute-opencl

lib_LTLIBRARIES = libpbkdf2-compute-opencl.la

AM_CXXFLAGS += -I$(srcdir)/../libhashspec-hashalgorithm/include

libpbkdf2_compute_opencl_la_LIBADD = ../libhashspec-hashalgorithm/libhashspec-hashalgorithm.la -l:libOpenCL.so.1
libpbkdf2_compute_opencl_la_SOURCES = \
    src/computecontext.cpp \
    src/kernelloader.cpp \
    src/devicecontext.cpp \
    src/hashfunctioncontext.cpp \
    src/processingunit.cpp \
    src/kernelloader.h \
    src/alignment.h

dist_data_DATA = data/opencl/kernels/pbkdf2_sha1.cl

SUBDIRS = . include
