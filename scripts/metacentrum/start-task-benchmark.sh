#!/bin/bash

HASH=$1
TASK=$2
MODE=$3
MACHINE=$4
DURATION=$5

if [ -z "$HASH" ]; then
    echo "ERROR: Hash algorithm not specified!" 1>&2
    exit 1
fi

if [ -z "$TASK" ]; then
    echo "ERROR: Task not specified!" 1>&2
    exit 1
fi

if [ -z "$MODE" ]; then
    echo "ERROR: Mode not specified!" 1>&2
    exit 1
fi

if [ -z "$MACHINE" ]; then
    echo "ERROR: Machine not specified!" 1>&2
    exit 1
fi

if [ "$MODE" = cpu ]; then
    EXTRA_ARGS=""
elif [ "$MODE" = gpu ]; then
    EXTRA_ARGS="#PBS -l gpu=1
#PBS -q gpu"
elif [ "$MODE" = 'gpu-cerit' ]; then
    EXTRA_ARGS="#PBS -l gpu=1
#PBS -q default@wagap.cerit-sc.cz"
    MODE='gpu'
else
    echo "ERROR: Invalid mode!" 1>&2
    exit 1
fi

if [ -z "$DURATION" ]; then
    DURATION=1d
fi

DEST_DIR=`pwd`

TASK_FILE=`mktemp`

cat >$TASK_FILE <<EOF
#!/bin/bash
#PBS -N benchmark-$HASH-$TASK-$MODE-$MACHINE
#PBS -l walltime=$DURATION
#PBS -l nodes=1:ppn=1:cl_$MACHINE
$EXTRA_ARGS

module use /storage/brno2/home/omos/packages/run.64/modules/modulefiles

module add gcc-4.8.4
module add libressl-2.2.0
module add cuda-6.5

cd /storage/brno2/home/omos/pbkdf2-gpu/scripts

bash run-benchmark.sh "$DEST_DIR" "$HASH" "$TASK" "$MODE" "$MACHINE" ../build "LD_PRELOAD=/afs/ics.muni.cz/software/cuda/6.5/cuda/lib64/libOpenCL.so"
EOF

qsub $TASK_FILE

rm -f $TASK_FILE
