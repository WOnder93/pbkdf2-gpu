dnl ---------------------------------------------------------------------
dnl Copyright (C) 2015, Ondrej Mosnacek <omosnacek@gmail.com>
dnl
dnl This program is free software: you can redistribute it and/or
dnl modify it under the terms of the GNU General Public License
dnl as published by the Free Software Foundation: either version 2
dnl of the License, or (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>.
dnl ---------------------------------------------------------------------

AC_CONFIG_MACRO_DIR([m4])

AC_INIT([pbkdf2-gpu], [0.1], [])
LT_INIT
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_SILENT_RULES([yes])

dnl # Check for the crypto library:
AC_CHECK_HEADER([openssl/evp.h], [], [AC_MSG_ERROR([You need the OpenSSL crypto library.])])
AC_CHECK_LIB([crypto], [EVP_DigestInit], [], [AC_MSG_ERROR([You need the OpenSSL crypto library.])])

dnl # Check for OpenCL 1.1:
AC_CHECK_HEADER([CL/cl.h], [], [AC_MSG_ERROR([You need the OpenCL library (version 1.1 or greater).])])
AC_CHECK_LIB([OpenCL], [clSetMemObjectDestructorCallback], [OPENCL_LIB=OpenCL], [
    AC_CHECK_LIB([:libOpenCL.so.1], [clSetMemObjectDestructorCallback], [OPENCL_LIB=:libOpenCL.so.1], [
        AC_MSG_ERROR([You need the OpenCL library (version 1.1 or greater).])
    ])
])

AC_SUBST([OPENCL_LIB])

AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11([noext], [mandatory])

AC_CONFIG_FILES([
    Makefile
    libhashspec-openssl/Makefile
    libhashspec-openssl/include/Makefile
    libhashspec-hashalgorithm/Makefile
    libhashspec-hashalgorithm/include/Makefile
    libpbkdf2-gpu-common/Makefile
    libpbkdf2-gpu-common/include/Makefile
    libpbkdf2-compute-cpu/Makefile
    libpbkdf2-compute-cpu/include/Makefile
    libpbkdf2-compute-opencl/Makefile
    libpbkdf2-compute-opencl/include/Makefile
    libcommandline/Makefile
    libcommandline/include/Makefile
    pbkdf2-compute-tests/Makefile
    benchmarking-tool/Makefile
    libcipherspec-cipheralgorithm/Makefile
    libcipherspec-cipheralgorithm/include/Makefile
    libivmode/Makefile
    libivmode/include/Makefile
    lukscrack-gpu/Makefile])

AC_OUTPUT
