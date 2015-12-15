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

AM_CXXFLAGS = -I$(srcdir)/src

bin_PROGRAMS = pbkdf2-compute-tests

AM_CXXFLAGS += -I$(srcdir)/../libpbkdf2-compute-opencl/include -I$(srcdir)/../libhashspec-hashalgorithm/include -I$(srcdir)/../libpbkdf2-gpu-common/include
AM_CXXFLAGS += -I$(srcdir)/../libpbkdf2-compute-cpu/include

pbkdf2_compute_tests_LDADD = ../libpbkdf2-compute-opencl/libpbkdf2-compute-opencl.la ../libpbkdf2-compute-cpu/libpbkdf2-compute-cpu.la -l$(OPENCL_LIB) -lpthread
pbkdf2_compute_tests_SOURCES = src/main.cpp \
    src/pbkdf2testvector.cpp \
    src/testsuite.cpp \
    src/tests.cpp \
    src/pbkdf2parameters.cpp \
    src/testexception.h \
    src/test.h \
    src/pbkdf2testvector.h \
    src/utils.h \
    src/testsuite.h \
    src/tests.h \
    src/pbkdf2parameters.h
