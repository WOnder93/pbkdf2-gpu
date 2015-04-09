AM_CXXFLAGS = -I$(srcdir)/src -I$(srcdir)/include/libivmode

lib_LTLIBRARIES = libivmode.la

AM_CXXFLAGS += -I$(srcdir)/../libhashspec-hashalgorithm/include
AM_CXXFLAGS += -I$(srcdir)/../libcipherspec-cipheralgorithm/include

libivmode_la_LIBADD = \
    ../libhashspec-hashalgorithm/libhashspec-hashalgorithm.la \
    ../libcipherspec-cipheralgorithm/libcipherspec-cipheralgorithm.la
libivmode_la_SOURCES = \
    src/ivgenerator.cpp \
    src/nullivgenerator.cpp \
    src/plainivgenerator.cpp \
    src/plain64ivgenerator.cpp \
    src/essivivgenerator.cpp \
    src/nullivgenerator.h \
    src/plainivgenerator.h \
    src/plain64ivgenerator.h \
    src/essivivgenerator.h

SUBDIRS = . include
