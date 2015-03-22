AM_CXXFLAGS = -I$(srcdir)/src -I$(srcdir)/include/libhashspec-hashalgorithm

lib_LTLIBRARIES = libhashspec-hashalgorithm.la

AM_CXXFLAGS += -I$(srcdir)/../libhashspec-openssl/include

libhashspec_hashalgorithm_la_LIBADD = ../libhashspec-openssl/libhashspec-openssl.la -lcrypto
libhashspec_hashalgorithm_la_SOURCES = \
    src/hashalgorithm.cpp

SUBDIRS = . include
