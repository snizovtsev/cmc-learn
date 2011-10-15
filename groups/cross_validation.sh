#!/bin/bash

# Example:
# ./cross_validation.sh -L approx_step 0.25 0.05 0.5

LANG=C

PARAM=$1
REPORT=$2
VALUES=`seq $3 $4 $5`

NFOLDS=5
FOLDS=`seq 1 $NFOLDS`

for value in $VALUES; do
    for TEST in $FOLDS; do
        TRAIN=`seq 1 $[$TEST-1]`' '`seq $[$TEST+1] $NFOLDS`
        echo $TRAIN | sed -r 's/([0-9])/\1\.idl/g' | xargs cat > train-$TEST.idl

        model=$TEST-$REPORT-$value.svc
        result=$TEST-$REPORT-$value.out
        ../bin/learn $PARAM $value train-$TEST.idl $model
        ../bin/predict $PARAM $value $model $TEST/*.png > $result
    done
done
