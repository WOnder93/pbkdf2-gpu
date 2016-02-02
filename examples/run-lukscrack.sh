#!/bin/bash

LUKSCRACK=$1
PWLIST=$2
BATCHSIZE=$3
HEADER=$4

"$1" --pwgen=list:"$PWLIST" -b $BATCHSIZE "$HEADER"
