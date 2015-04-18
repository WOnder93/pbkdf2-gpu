#!/bin/bash

BIN_DIR="$1"  # pbkdf2-gpu root build directory
DEST_DIR="$2" # destination directory for benchmark logs/data
ID="$3"       # benchmark id (will appear in log/data filenames)
ENV="$4"      # environment variables to be passed to benchmarking-tool (will be passed to the 'env' command)

if [ -z "$BIN_DIR" ]; then
    echo "ERROR: Build directory not specified!" 1>&2
    exit 1
fi

if [ -z "$DEST_DIR" ]; then
    DEST_DIR=.
    echo "NOTE: Using current directory as destination for log files..." 1>&2
fi

if [ -z "$ID" ]; then
    ID="local"
    echo "NOTE: Using '$ID' as benchmark ID..."
fi

# $ENV is empty by default

echo "Running GPU benchmarks..."
for task in dl-iter-bs salt-len; do
    echo "Running benchmark '$task'..."
    bash run-benchmark.sh "$DEST_DIR" "$task" gpu "$ID" "$BIN_DIR" "$ENV"
done
