#!/bin/bash
# ---------------------------------------------------------------------
# Copyright (C) 2015, Ondrej Mosnacek <omosnacek@gmail.com>
#
# This program is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation: either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# ---------------------------------------------------------------------

DEST_DIR=$1
HASH=$2
TASK=$3
MODE=$4
ID=$5
BINDIR=$6
ENV=$7

run_bt() {
    (cd "$BINDIR/benchmarking-tool" && env $ENV ./benchmarking-tool $*)
}

if [ "$MODE" = cpu ]; then
    SAMPLES=20

    DEFAULT_ITERATIONS=4096
    DEFAULT_DK_LENGTH=16
    DEFAULT_BATCH_SIZE=256
    DEFAULT_SALT=$(printf "%032x" 0)

    DL_FROM=20
    DL_TO=$((20 * 32))
    ITER_FROM=1
    ITER_TO=8192
    BS_FROM=32
    BS_TO=8192
    SL_FROM=32
    SL_TO=65536

    BT_MODE="cpu"
    DEV_INFO_CMD="cat /proc/cpuinfo"
elif [ "$MODE" = gpu ]; then
    SAMPLES=10

    DEFAULT_ITERATIONS=16384
    DEFAULT_DK_LENGTH=16
    DEFAULT_BATCH_SIZE=65536
    DEFAULT_SALT=$(printf "%032x" 0)

    DL_FROM=20
    DL_TO=$((20 * 32))
    ITER_FROM=1
    ITER_TO=65536
    BS_FROM=1024
    BS_TO=$((256 * 1024))
    SL_FROM=32
    SL_TO=65536

    BT_MODE="opencl"
    DEV_INFO_CMD="run_bt --mode=opencl --list-devices"
else
    echo "ERROR: invalid mode: $MODE" 1>&2
    exit 1
fi

DATA_FILE="$DEST_DIR/benchmark-$HASH-$TASK-$MODE-$ID.csv"
LOG_FILE="$DEST_DIR/benchmark-$HASH-$TASK-$MODE-$ID.log"

print_dev_info() {
    echo "DEFAULT_ITERATIONS=$DEFAULT_ITERATIONS"
    echo "DEFAULT_DK_LENGTH=$DEFAULT_DK_LENGTH"
    echo "DEFAULT_BATCH_SIZE=$DEFAULT_BATCH_SIZE"
    echo
    echo "BEGIN Device info"
    $DEV_INFO_CMD
    echo "END   Device info"
}

run_benchmark() {
    HASH=$1
    SAMPLES=$2
    SALT=$3
    ITER=$4
    DK_LENGTH=$5
    BATCH_SIZE=$6
    run_bt --mode=$BT_MODE --hash-spec="$HASH" --output-mode=raw --samples $SAMPLES --salt "$SALT" -i "$ITER" -k "$DK_LENGTH" -b "$BATCH_SIZE"
}

rm -f "$DATA_FILE"
rm -f "$LOG_FILE"

print_dev_info >> $LOG_FILE

case "$TASK" in
    simple)
        for sample in `run_benchmark $HASH $SAMPLES "$DEFAULT_SALT" $DEFAULT_ITERATIONS $DEFAULT_DK_LENGTH $DEFAULT_BATCH_SIZE`; do
            echo -n "," >> $DATA_FILE
            echo -n "$sample" >> $DATA_FILE
        done
        echo >> $DATA_FILE
        ;;
    dl-iter-bs)
        for (( dl = $DL_FROM; dl <= $DL_TO; dl *= 2 )); do
            for (( it = $ITER_FROM; it <= $ITER_TO; it *= 2 )); do
                for (( bs = $BS_FROM; bs <= $BS_TO; bs *= 2 )); do
                    echo -n "$dl,$it,$bs" >> $DATA_FILE
                    for sample in `run_benchmark $HASH $SAMPLES "$DEFAULT_SALT" $it $dl $bs`; do
                        echo -n "," >> $DATA_FILE
                        echo -n "$sample" >> $DATA_FILE
                    done
                    echo >> $DATA_FILE
                done
            done
        done
        ;;
    dl-bs)
        for (( dl = $DL_FROM; dl <= $DL_TO; dl *= 2 )); do
            for (( bs = $BS_FROM; bs <= $BS_TO; bs *= 2 )); do
                echo -n "$dl,$bs" >> $DATA_FILE
                for sample in `run_benchmark $HASH $SAMPLES "$DEFAULT_SALT" $DEFAULT_ITERATIONS $dl $bs`; do
                    echo -n "," >> $DATA_FILE
                    echo -n "$sample" >> $DATA_FILE
                done
                echo >> $DATA_FILE
            done
        done
        ;;
    salt-len)
        for (( sl = $SL_FROM; sl <= $SL_TO; sl *= 2 )); do
            echo -n "$sl" >> $DATA_FILE
            SALT=`head -c $sl /dev/zero | tr '\0' 'x'`
            for sample in `run_benchmark $HASH $SAMPLES "$SALT" $DEFAULT_ITERATIONS $DEFAULT_DK_LENGTH $DEFAULT_BATCH_SIZE`; do
                echo -n "," >> $DATA_FILE
                echo -n "$sample" >> $DATA_FILE
            done
            echo >> $DATA_FILE
        done
        ;;
    iterations)
        for (( it = $ITER_FROM; it <= $ITER_TO; it *= 2 )); do
            echo -n "$it" >> $DATA_FILE
            for sample in `run_benchmark $HASH $SAMPLES "$DEFAULT_SALT" $it $DEFAULT_DK_LENGTH $DEFAULT_BATCH_SIZE`; do
                echo -n "," >> $DATA_FILE
                echo -n "$sample" >> $DATA_FILE
            done
            echo >> $DATA_FILE
        done
        ;;
    dk-length)
        for (( dl = $DL_FROM; dl <= $DL_TO; dl *= 2 )); do
            echo -n "$dl" >> $DATA_FILE
            for sample in `run_benchmark $HASH $SAMPLES "$DEFAULT_SALT" $DEFAULT_ITERATIONS $dl $DEFAULT_BATCH_SIZE`; do
                echo -n "," >> $DATA_FILE
                echo -n "$sample" >> $DATA_FILE
            done
            echo >> $DATA_FILE
        done
        ;;
    batch-size)
        for (( bs = $BS_FROM; bs <= $BS_TO; bs *= 2 )); do
            echo -n "$bs" >> $DATA_FILE
            for sample in `run_benchmark $HASH $SAMPLES "$DEFAULT_SALT" $DEFAULT_ITERATIONS $DEFAULT_DK_LENGTH $bs`; do
                echo -n "," >> $DATA_FILE
                echo -n "$sample" >> $DATA_FILE
            done
            echo >> $DATA_FILE
        done
        ;;
    *)
        echo "ERROR: invalid task: $TASK" 1>&2
        exit 1
        ;;
esac
