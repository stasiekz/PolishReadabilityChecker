#!/bin/bash

if [ ${#} -eq 0 ]
then
	echo -e "\e[1;31mUsage: $0 corpus2class\e[0m"
	exit 1
fi

UPPER=`echo ${1} | tr '[:lower:]' '[:upper:]'`
LOWER=`echo ${1} | tr '[:upper:]' '[:lower:]'`
CLNAM=${1}
OUTFILE=${LOWER}.i

cat swig_template_corpus2.template  | sed s/TEMPLATE/${UPPER}/g | sed s/Template/${CLNAM}/g | sed s/template/${LOWER}/g > ${OUTFILE}

echo -e "\e[1;32m${OUTFILE} has been generated. Don't forget to add ${OUTFILE} to Makefile (or CMake)!\e[0m"
