#!/bin/bash

# Example:
# ./gen_report.sh approx_step

LANG=C

NFOLDS=5
FOLDS=`seq 1 $NFOLDS`

REPORT=$1
FOUND_VALUES=`ls ?-$REPORT-*.out | sed -r s/[1-$NFOLDS]-$REPORT-\(.\*\)\\\\.out/\\\\1/g | sort -nu`
VALUES=""
# Check if values is ready
for value in $FOUND_VALUES; do
	append=true
	for fold in $FOLDS; do
		[ -f $fold-$REPORT-$value.out ] || append=false
	done
	( $append ) && VALUES="$VALUES $value"
done

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
