AM_CXXFLAGS = -I$(srcdir)/src

bin_PROGRAMS = benchmarking-tool

AM_CXXFLAGS += -I$(srcdir)/../libpbkdf2-compute-opencl/include -I$(srcdir)/../libhashspec-hashalgorithm/include
AM_CXXFLAGS += -I$(srcdir)/../libpbkdf2-compute-cpu/include
AM_CXXFLAGS += -I$(srcdir)/../libcommandline/include

benchmarking_tool_LDADD = ../libpbkdf2-compute-opencl/libpbkdf2-compute-opencl.la ../libpbkdf2-compute-cpu/libpbkdf2-compute-cpu.la ../libcommandline/libcommandline.la -l$(OPENCL_LIB)
benchmarking_tool_SOURCES = src/main.cpp \
    src/benchmark.h

DATA_PATH = $(srcdir)/../libpbkdf2-compute-opencl

all-local:
	rm -rf data && $(LN_S) $(DATA_PATH)/data data

clean-local:
	rm -rf data
