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

bin_PROGRAMS = lukscrack-gpu

AM_CXXFLAGS += -I$(srcdir)/../libhashspec-hashalgorithm/include
AM_CXXFLAGS += -I$(srcdir)/../libcipherspec-cipheralgorithm/include
AM_CXXFLAGS += -I$(srcdir)/../libivmode/include
AM_CXXFLAGS += -I$(srcdir)/../libcommandline/include
AM_CXXFLAGS += -I$(srcdir)/../libpbkdf2-compute-opencl/include -I$(srcdir)/../libpbkdf2-gpu-common/include

lukscrack_gpu_LDADD = -l$(OPENCL_LIB) -lpthread \
    ../libhashspec-hashalgorithm/libhashspec-hashalgorithm.la \
    ../libcipherspec-cipheralgorithm/libcipherspec-cipheralgorithm.la \
    ../libivmode/libivmode.la \
    ../libcommandline/libcommandline.la \
    ../libpbkdf2-compute-opencl/libpbkdf2-compute-opencl.la

lukscrack_gpu_SOURCES = src/main.cpp \
    src/lukscrack.cpp \
    src/passworddata.cpp \
    src/passwordgenerator.cpp \
    src/listpasswordgenerator.cpp \
    src/sectordecryptor.cpp \
    src/afmerger.cpp \
    src/threadpool.cpp \
    src/gpu/batchprocessingcontext.cpp \
    src/gpu/crackingcontext.cpp \
    src/gpu/devicecrackingcontext.cpp \
    src/lukscrack.h \
    src/passworddata.h \
    src/passwordgenerator.h \
    src/listpasswordgenerator.h \
    src/passworddistributor.h \
    src/sectordecryptor.h \
    src/afmerger.h \
    src/threadpool.h \
    src/gpu/batchprocessingcontext.h \
    src/gpu/crackingcontext.h \
    src/gpu/devicecrackingcontext.h
