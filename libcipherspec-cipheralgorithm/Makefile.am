AM_CXXFLAGS = -I$(srcdir)/src -I$(srcdir)/include/libcipherspec-cipheralgorithm

lib_LTLIBRARIES = libcipherspec-cipheralgorithm.la

libcipherspec_cipheralgorithm_la_LIBADD = -lcrypto
libcipherspec_cipheralgorithm_la_SOURCES = \
    src/cipheralgorithm.cpp

SUBDIRS = . include
