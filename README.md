# PBKDF2 bulk computation utilities
A C++11 library for brute-forcing PBKDF2 using (not only) massive parallel processing on GPU hardware and a tool for brute-forcing LUKS passwords.

This project is part of my [bachelor's thesis](https://is.muni.cz/th/409879/fi_b/?lang=en) on the [Faculty of Informatics, Masaryk University](https://www.fi.muni.cz).

## Building

### Prerequisites

 * The [OpenSSL](https://www.openssl.org/)/[LibreSSL](http://www.libressl.org/) `crypto` library. Note: LibreSSL is significantly faster for PBKDF2 computation on the CPU.
 * An [OpenCL](https://www.khronos.org/opencl/) implementation (you should have `libOpenCL.so` or `libOpenCL.so.1` somewhere in your library search path).

### Using GNU autotools

```bash
$ autoreconf -i
$ mkdir build && cd build
$ ../configure --disable-shared && make
```

### Using qmake

The project also ships with qmake project files so you can build it using [qmake](http://doc.qt.io/qt-4.8/qmake-manual.html) or open it in the [QtCreator IDE](http://wiki.qt.io/Category:Tools::QtCreator) (just load the `pbkdf2-gpu.pro` file).

Building from terminal using qmake:

```bash
$ mkdir build-qmake && cd build-qmake
$ qmake ../pbkdf2-gpu.pro && make
```

## Subprojects
 * **libhashspec-openssl** &ndash; A utility library to lookup an OpenSSL hash algorithm (a pointer to `EVP_MD` structure) from a LUKS hashspec string (see the [LUKS On-Disk Format Specification](https://gitlab.com/cryptsetup/cryptsetup/wikis/LUKS-standard/on-disk-format.pdf) for more information).
 * **libhashspec-hashalgorithm** &ndash; A utility library to lookup a hash algorithm implementation based on a LUKS hashspec string (see above).
 * **libcipherspec-cipheralgorithm** &ndash; A utility library to lookup a cipher algorithm implementation based on LUKS cipherspec and ciphermode strings (see above).
 * **libivmode** &ndash; A utility library to lookup an IV generator implementation based on a LUKS ivmode string (see above).
 * **libpbkdf2-compute-cpu** &ndash; A reference implementation of the libpbkdf2-compute interface (see below) performing computation on the CPU.
 * **libpbkdf2-compute-opencl** &ndash; An implementation of the libpbkdf2-compute interface (see below) performing computation on one or more OpenCL devices.
 * **libcommandline** &ndash; A simple command-line argument parser.
 * **pbkdf2-compute-tests** &ndash; A utility that runs tests (currently only checks computation of RFC test vectors) on the libpbkdf2-compute-\* libraries.
 * **benchmarking-tool** &ndash; A command-line tool for benchmarking the performance of the libpbkdf2-compute-\* libraries.
 * **lukscrack-gpu** &ndash; A command-line tool for cracking passwords of LUKS disk partitions.

## Supported algorithms
### Hash functions
All hash functions specified in the [LUKS On-Disk Format Specification](https://gitlab.com/cryptsetup/cryptsetup/wikis/LUKS-standard/on-disk-format.pdf) are supported:
 * SHA-1 (`sha1`)
 * SHA-256 (`sha256`)
 * SHA-512 (`sha512`)
 * RIPEMD-160 (`ripemd160`)

### Cipher algorithms and modes
 * AES-128 (`aes`) -- ECB, CBC, XTS
 * AES-192 (`aes`) -- ECB, CBC, XTS
 * AES-256 (`aes`) -- ECB, CBC, XTS
 * CAST-128 (`cast5`) -- ECB, CBC

TwoFish (`twofish`), Serpent (`serpent`) and CAST-256 (`cast6`) are not supported.

### IV generators
 * (none) - if IV mode is not specified (only for ECB mode)
 * `null`
 * `plain`
 * `plain64`
 * `essiv:<hash>` where `<hash>` is one of the hash functions listed above.

## The common interface of libpbkdf2-compute-\* libraries

TODO
