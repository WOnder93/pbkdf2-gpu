#!/bin/bash

cd `dirname $0` || exit 1

for machine in manegrot; do
    for task in batch-size iterations dk-length salt-len; do
        echo -n "Starting task '$task' on '$machine'... "
        bash "start-task-benchmark.sh" "$task" "cpu" "$machine"
    done
done
