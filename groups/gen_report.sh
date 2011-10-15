#!/bin/bash

# Example:
# ./gen_report.sh approx_step 0.25 0.05 0.5

LANG=C

REPORT=$1
VALUES=`seq $2 $3 $4`

NFOLDS=5
FOLDS=`seq 1 $NFOLDS`

rm $REPORT.data

for value in $VALUES; do
	sum=0.0
	for fold in $FOLDS; do
		cur=`../bin/rate $fold.idl $fold-$REPORT-$value.out | tail -n 1 | cut -f 2 | sed 's/%//'`
		sum=`echo $sum + $cur | bc`
	done
	avg=`echo "scale=5; $sum / $NFOLDS" | bc`
	echo $value $avg >> $REPORT.data
done
