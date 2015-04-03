TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = \
    libhashspec-openssl \
    libhashspec-hashalgorithm \
    libpbkdf2-compute-cpu \
    libpbkdf2-compute-opencl \
    libcommandline \
    pbkdf2-compute-tests \
    libcipherspec-cipheralgorithm \
    libivmode \
    lukscrack-gpu \
    benchmarking-tool
