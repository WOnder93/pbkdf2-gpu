#!/bin/bash

SCRIPTS_DIR="$(dirname $0)"

MACHINES="$1"
HASHES="$2"
TASKS="$3"

if [ "$MACHINES" = "all" ]; then
    MACHINES="zubat doom gram konos"
fi

if [ "$HASHES" = "all" ]; then
    HASHES="ripemd160 sha1 sha256 sha512"
fi

if [ "$TASKS" = "all" ]; then
    TASKS="batch-size iterations salt-len"
fi

for machine in $MACHINES; do
    for hash in $HASHES; do
        for task in $TASKS; do
            echo -n "Starting task '$task' on '$machine' for hash '$hash'... "
            bash "$SCRIPTS_DIR/start-task-benchmark.sh" "$hash" "$task" "gpu" "$machine"
        done
    done
done
