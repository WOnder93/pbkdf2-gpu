# Scripts

## Running benchmarks

 * **run-benchmark.sh** &ndash; A generic script for running benchmarks.
 * **run-benchmarks-cpu/gpu.sh** &ndash; Runs standard CPU/GPU benchmarks. See comments inside for decription of the command-line arguments.
 * **run-benchmarks-all.sh** &ndash; Runs both CPU and GPU benchmarks (forwards arguments to the two scripts above).

The scripts always use the first GPU device that OpenCL returns. I didn't have time to add an argument for that yet.

NOTE: The GPU benchmarks might take a really long time to run (up to several hours).
