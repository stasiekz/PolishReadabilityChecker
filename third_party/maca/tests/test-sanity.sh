#!/bin/bash

TOKI_DATA=@LIBMACA_SRC_DATA_DIR@
TEST_DATA=@PROJECT_SOURCE_DIR@

if [ "$2" == "-a" ]; then
	EXIT=0
else
	EXIT=1
fi

for config in morfeusz-nkjp-official; do
for i in $TEST_DATA/$1/*.txt; do
	maca-analyse -C $TOKI_DATA $config -q -o orth,actual_ws,end_nl --initial none < $i > $i.tok
	if diff $i $i.tok > /dev/null; then
		echo "OK: $i"
		rm $i.tok
	else
		echo "ERROR: Difference in $i [$tagset]"
		if [ $EXIT -eq 1 ]; then
			exit 1
		fi
	fi
done
done

echo "*** All sanity tests passed"
