#!/bin/bash

LUKSCRACK=$1
PWLIST=$2
BATCHSIZE=$3
HEADER=$4

"$1" --opencl-data-dir=../libpbkdf2-compute-opencl/data --pwgen=list:"$PWLIST" -b $BATCHSIZE "$HEADER"
