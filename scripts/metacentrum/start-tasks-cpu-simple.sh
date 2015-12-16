#!/bin/bash

SCRIPTS_DIR="$(dirname $0)"

for machine in manegrot mandos losgar konos minos luna hermes; do
    for hash in sha1 sha256; do
        echo -n "Starting task 'simple' on '$machine' for hash '$hash'... "
        bash "$SCRIPTS_DIR/start-task-benchmark.sh" "$hash" "simple" "cpu" "$machine"
    done
done
