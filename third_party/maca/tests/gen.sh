#!/bin/bash
if [ "$1" == "" ]; then
	BYTES=10204
else
	BYTES=$1
fi
ASC="tr -dc \"[:cntrl:]A-Za-z0-9.,!()[]\\n{}\\\"\';:\""
if [ "$2" == "pl" ]; then
	ASC="tr -dc \"[:cntrl:][:print:][\\241-\\377\""
fi
cat /dev/urandom | \
	$ASC | \
	tr "[:cntrl:]" " " | \
	head -c $BYTES | \
	sed "s/   /\n/g" | \
	sed "s/^ \+//" | \
	sed "s/ \+$//" | \
	sed "/^$/d" | \
	sed "\${/^$/!s/$/\n/;}" | \
	iconv -c -f latin2 -t utf8//TRANSLIT
