AM_CXXFLAGS = -I$(srcdir)/src -I$(srcdir)/include/libhashspec-openssl

lib_LTLIBRARIES = libhashspec-openssl.la

libhashspec_openssl_la_LIBADD = -lcrypto
libhashspec_openssl_la_SOURCES = \
    src/digestlookup.cpp

SUBDIRS = . include
