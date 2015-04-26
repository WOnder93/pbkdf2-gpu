#!/bin/bash

cd `dirname $0` || exit 1

for machine in manegrot mandos losgar konos minos luna hermes; do
    echo -n "Starting task 'simple' on '$machine'... "
    bash "start-task-benchmark.sh" "simple" "cpu" "$machine"
done
