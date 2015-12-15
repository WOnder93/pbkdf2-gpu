# ---------------------------------------------------------------------
# Copyright (C) 2015, Ondrej Mosnacek <omosnacek@gmail.com>
#
# This program is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation: either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# ---------------------------------------------------------------------

AM_CXXFLAGS = -I$(srcdir)/src -I$(srcdir)/include/libpbkdf2-compute-opencl

lib_LTLIBRARIES = libpbkdf2-compute-opencl.la

AM_CXXFLAGS += -I$(srcdir)/../libpbkdf2-gpu-common/include
AM_CXXFLAGS += -I$(srcdir)/../libhashspec-hashalgorithm/include

libpbkdf2_compute_opencl_la_LIBADD = ../libhashspec-hashalgorithm/libhashspec-hashalgorithm.la -l$(OPENCL_LIB)
libpbkdf2_compute_opencl_la_SOURCES = \
    src/device.cpp \
    src/globalcontext.cpp \
    src/kernelloader.cpp \
    src/hashfunctioncontext.cpp \
    src/computecontext.cpp \
    src/devicecontext.cpp \
    src/processingunit.cpp \
    src/hashfunctionhelper.cpp \
    src/kernelgenerator.cpp \
    src/sha1hashfunctionhelper.cpp \
    src/sha2hashfunctionhelper.cpp \
    src/sha256hashfunctionhelper.cpp \
    src/sha512hashfunctionhelper.cpp \
    src/ripemd160hashfunctionhelper.cpp \
    src/openclwriter.cpp \
    src/kernelloader.h \
    src/alignment.h \
    src/kernelgenerator.h \
    src/hashfunctionhelper.h \
    src/sha1hashfunctionhelper.h \
    src/sha2hashfunctionhelper.h \
    src/sha256hashfunctionhelper.h \
    src/sha512hashfunctionhelper.h \
    src/ripemd160hashfunctionhelper.h \
    src/openclwriter.h

SUBDIRS = . include
