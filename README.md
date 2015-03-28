# PBKDF2 bulk computation utilities
A C++11 library for brute-forcing PBKDF2 using (not only) massive parallel processing on GPU hardware and a tool for brute-forcing LUKS passwords.

This project is part of my bachelor's thesis on the [Faculty of Informatics, Masaryk University](https://www.fi.muni.cz).

## Building

### Prerequisites

 * The [OpenSSL](https://www.openssl.org/)/[LibreSSL](http://www.libressl.org/) `crypto` library.
 * An [OpenCL](https://www.khronos.org/opencl/) implementation (you should have libOpenCL.so.1 somewhere in your library search path).
 * If you are building using GCC, make sure use at least version 4.8.1 because 4.7.2 gives a weird error when compiling `libpbkdf2-compute-opencl/src/device.cpp` (other versions between 4.7.2 and 4.8.1 might also work).

### Using GNU autotools

```bash
$ autoreconf -i
$ mkdir build && cd build
$ ../configure --disable-shared && make
```

NOTE: When running executables always run them in their containing directory so they can find the OpenCL kernel source files!

### Using qmake

The project also ships with qmake project files so you can build it using [qmake](http://doc.qt.io/qt-4.8/qmake-manual.html) or open it in the [QtCreator IDE](http://wiki.qt.io/Category:Tools::QtCreator) (just load the `pbkdf2-gpu.pro` file). Note that when running programs that use the `libpbkdf2-compute-opencl` library from QtCreator, you must set the current directory to `(project root)/pbkdf2-gpu/libpbkdf2-compute-opencl` so they can find the OpenCL kernel source files.

Building from terminal using qmake:

```bash
$ mkdir build-qmake && cd build-qmake
$ qmake ../pbkdf2-gpu.pro && make
$ # I'm not sure how to do this portably in the .pro files:
$ for i in pbkdf2-compute-tests benchmarking-tool lukscrack-gpu; do (cd $i && ln -s ../../libpbkdf2-compute-opencl/data data); done
```

## Subprojects
 * **libhashspec-openssl** &ndash; A utility library to lookup OpenSSL hash algorithm (a pointer to `EVP_MD` structure) from a LUKS hashspec (see the [LUKS On-Disk Format Specification](https://gitlab.com/cryptsetup/cryptsetup/wikis/LUKS-standard/on-disk-format.pdf) for more information).
 * **libhashspec-hashalgorithm** &ndash; A utility library to lookup a hash algorithm implementation from a LUKS hashspec (see above).
 * **libpbkdf2-compute-cpu** &ndash; A reference implementation of the libpbkdf2-compute interface (see below) performing computation on the CPU.
 * **libpbkdf2-compute-opencl** &ndash; An implementation of the libpbkdf2-compute interface (see below) performing computation on one or more OpenCL devices.
 * **pbkdf2-compute-tests** &ndash; A utility that runs tests (currently only checks computation of RFC test vectors) on libpbkdf2-compute-\* libraries.
 * **benchmarking-tool** &ndash; A command-line tool for benchmarking the speed of computation (currently only has basic functionality).
 * **lukscrack-gpu** &ndash; A command-line tool for cracking passwords of LUKS disk partitions (currently a work-in-progress).

## The common interface of libpbkdf2-compute-\* libraries

TODO
