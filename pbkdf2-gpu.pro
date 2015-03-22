TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = \
	libhashspec-openssl \
	libhashspec-hashalgorithm \
	libpbkdf2-compute-cpu \
	libpbkdf2-compute-opencl \
	pbkdf2-compute-tests \
	lukscrack-gpu \
	benchmarking-tool
