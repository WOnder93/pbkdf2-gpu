#!/bin/bash

DEST_DIR=$1
TASK=$2
MODE=$3
ID=$4
BINDIR=$5
ENV=$6

run_bt() {
    (cd "$BINDIR/benchmarking-tool" && env $ENV ./benchmarking-tool $*)
}

if [ "$MODE" = cpu ]; then
    SAMPLES=20

    DEFAULT_ITERATIONS=4096
    DEFAULT_DK_LENGTH=16
    DEFAULT_BATCH_SIZE=256

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

DATA_FILE="$DEST_DIR/benchmark-$TASK-$MODE-$ID.csv"
LOG_FILE="$DEST_DIR/benchmark-$TASK-$MODE-$ID.log"

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
    SAMPLES=$1
    SALT=$2
    ITER=$3
    DK_LENGTH=$4
    BATCH_SIZE=$5
    run_bt --mode=$BT_MODE --output-mode=raw --samples $SAMPLES --salt "$SALT" -i "$ITER" -k "$DK_LENGTH" -b "$BATCH_SIZE"
}

rm -f "$DATA_FILE"
rm -f "$LOG_FILE"

print_dev_info >> $LOG_FILE

case "$TASK" in
    dl-iter-bs)
        for (( dl = $DL_FROM; dl <= $DL_TO; dl *= 2 )); do
            for (( it = $ITER_FROM; it <= $ITER_TO; it *= 2 )); do
                for (( bs = $BS_FROM; bs <= $BS_TO; bs *= 2 )); do
                    echo -n "$dl,$it,$bs" >> $DATA_FILE
                    for sample in `run_benchmark $SAMPLES "$SALT" $it $dl $bs`; do
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
                for sample in `run_benchmark $SAMPLES "$SALT" $DEFAULT_ITERATIONS $dl $bs`; do
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
            for sample in `run_benchmark $SAMPLES "$SALT" $DEFAULT_ITERATIONS $DEFAULT_DK_LENGTH $DEFAULT_BATCH_SIZE`; do
                echo -n "," >> $DATA_FILE
                echo -n "$sample" >> $DATA_FILE
            done
            echo >> $DATA_FILE
        done
        ;;
    iterations)
        for (( it = $ITER_FROM; it <= $ITER_TO; it *= 2 )); do
            echo -n "$it" >> $DATA_FILE
            for sample in `run_benchmark $SAMPLES "$DEFAULT_SALT" $it $DEFAULT_DK_LENGTH $DEFAULT_BATCH_SIZE`; do
                echo -n "," >> $DATA_FILE
                echo -n "$sample" >> $DATA_FILE
            done
            echo >> $DATA_FILE
        done
        ;;
    dk-length)
        for (( dl = $DL_FROM; dl <= $DL_TO; dl *= 2 )); do
            echo -n "$dl" >> $DATA_FILE
            for sample in `run_benchmark $SAMPLES "$DEFAULT_SALT" $DEFAULT_ITERATIONS $dl $DEFAULT_BATCH_SIZE`; do
                echo -n "," >> $DATA_FILE
                echo -n "$sample" >> $DATA_FILE
            done
            echo >> $DATA_FILE
        done
        ;;
    batch-size)
        for (( bs = $BS_FROM; bs <= $BS_TO; bs *= 2 )); do
            echo -n "$bs" >> $DATA_FILE
            for sample in `run_benchmark $SAMPLES "$DEFAULT_SALT" $DEFAULT_ITERATIONS $DEFAULT_DK_LENGTH $bs`; do
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
