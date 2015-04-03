#!/bin/bash

if [ "x$1" == "x" ]; then
    echo "usage: run-whatever.sh <path-to-lukscrack-gpu>"
    exit 1
fi

./run-lukscrack.sh $1 hunter2-4096.pwlist 512 hunter2.luks
