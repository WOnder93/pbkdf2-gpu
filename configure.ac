AC_CONFIG_MACRO_DIR([m4])

AC_INIT([pbkdf2-gpu], [0.1], [])
LT_INIT
AM_INIT_AUTOMAKE([foreign subdir-objects])

AC_CONFIG_FILES([
    Makefile
    libhashspec-openssl/Makefile
    libhashspec-openssl/include/Makefile
    libhashspec-hashalgorithm/Makefile
    libhashspec-hashalgorithm/include/Makefile
    libpbkdf2-compute-cpu/Makefile
    libpbkdf2-compute-cpu/include/Makefile
    libpbkdf2-compute-opencl/Makefile
    libpbkdf2-compute-opencl/include/Makefile
    pbkdf2-compute-tests/Makefile
    benchmarking-tool/Makefile
    lukscrack-gpu/Makefile])

dnl 'ln -s' (or equivalent) needed for copying the data folder:
AC_PROG_LN_S

AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11([noext], [mandatory])

AC_OUTPUT
