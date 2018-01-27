#!/bin/bash
BUILDDIR=qtcreator-build
PROJECTS="corpus2 toki maca"
BASEDIR=../..
CURDIR=`pwd`
FILTER="grep -i -C 4 error"
#FILTER=cat

function fail() {
	echo "!!! $1 in $i !!!"
	cd $CURDIR
	exit 1
}

usage()
{
cat << EOF
usage: $0 options

OPTIONS:
   -h      Show this message
   -c      make clean before building
   -e      rm CMakeCache before building
   -n      skip make install step
   -p      pull from git repo first
EOF
}

OPT_CLEAN=
OPT_CACHE=
OPT_INSTALL=1
OPT_PULL=
while getopts "hcenp" OPTION
do
     case $OPTION in
         h)
             usage
             exit 0
             ;;
         c)
             OPT_CLEAN=1
             ;;
         e)
             OPT_CACHE=1
             ;;
         n)
             OPT_INSTALL=
             ;;
         p)
             OPT_PULL=1
             ;;
     esac
done

for i in $PROJECTS; do
	cd $BASEDIR/$i || fail  "cd error"
	if [[ ! -d $BUILDDIR ]]; then
		echo "Creating $BUILDDIR subdirectory in $i"
		mkdir $BUILDDIR
	fi
	cd $BUILDDIR
	pwd
	if [[ ! -z $OPT_PULL ]]; then
		echo "--- Pulling $i..."
		git pull
	fi
	echo "--- Configuring $i..."
	if [[ ! -z $OPT_CACHE ]]; then
		echo "--- rm CMakeCache.txt in $i ..."
		rm CMakeCache.txt
	fi
	cmake .. > /dev/null || fail "cmake error"
	if [[ ! -z $OPT_CLEAN ]]; then
		echo "--- Cleaning $i..."
		make clean > /dev/null || fail "make clean error"
	fi
	echo "--- Building $i..."
	make -j4 > /dev/null || fail  "make error"
	if [[ ! -z $OPT_INSTALL ]]; then
		echo "--- Installing $i..."
		sudo make install > /dev/null || fail "make install error"
	fi
	sudo ldconfig
	echo "--- Testing $i..."
	make test > /dev/null || fail "make test error in"
done
cd $CURDIR

