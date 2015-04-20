#!/bin/bash

cd `dirname $0` || exit 1

for machine in doom gram konos; do
    for task in dl-bs iterations salt-len; do
        echo -n "Starting task '$task' on '$machine'... "
        bash "start-task-benchmark.sh" "$task" "gpu" "$machine"
    done
done
