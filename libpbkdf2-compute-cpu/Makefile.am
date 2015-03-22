AM_CXXFLAGS = -I$(srcdir)/src -I$(srcdir)/include/libpbkdf2-compute-cpu

lib_LTLIBRARIES = libpbkdf2-compute-cpu.la

AM_CXXFLAGS += -I$(srcdir)/../libhashspec-hashalgorithm/include -I$(srcdir)/../libhashspec-openssl/include

libpbkdf2_compute_cpu_la_LIBADD = ../libhashspec-hashalgorithm/libhashspec-hashalgorithm.la -lpthread -lcrypto
libpbkdf2_compute_cpu_la_SOURCES = \
    src/hashfunctioncontext.cpp \
    src/processingunit.cpp \
    src/pkcs_pbkdf2.cpp \
    src/sha1.cpp \
    src/sha1.h \
    src/pkcs_pbkdf2.h

SUBDIRS = . include
