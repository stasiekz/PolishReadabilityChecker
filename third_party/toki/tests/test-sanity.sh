#!/bin/bash

TOKI_DATA=@LIBTOKI_SRC_DATA_DIR@
TEST_DATA=@PROJECT_SOURCE_DIR@

if [ "$2" == "-a" ]; then
	EXIT=0
else
	EXIT=1
fi

for i in $TEST_DATA/$1/*.txt; do
	toki-app -C $TOKI_DATA -q -n -f '$ws_ws$orth' -i none < $i > $i.tok
	if diff $i $i.tok > /dev/null; then
		echo "OK: $i"
		rm $i.tok
	else
		echo "ERROR: Difference in $i"
		if [ $EXIT -eq 1 ]; then
			exit 1
		fi
	fi
done

echo "*** All sanity tests passed"
