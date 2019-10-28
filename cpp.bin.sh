#!/bin/sh

set -e

# check for $1 parameter
if [ "$#" -ne "1" ]; then
    echo "Usage : $0 [Filename.cpp]"
    exit 0;
fi

# file check exists
[ -e "$1" ] || [ -e "$1.cpp" ] || { echo "No such file \"$1\"" ; exit -1; };

# make the thing
if [ -f "$1" ]; then
    if echo $1 | grep "[.]cpp" > /dev/null ; then
	echo "make ${1/cpp/bin}"
	make "${1/cpp/bin}"
    else
	echo "make $1.bin"
	make "$1.bin"
    fi
elif [ -f "$1.cpp" ]; then
    echo "make ${1/cpp/bin}"
    make "${1/cpp/bin}"
elif [ -d "$1" ];then
    echo "$1/make"
    pushd "$1"
    make
    popd
    [ -e "$1/${1/cpp/}.bin" ] && mv "$1/${1/cpp/}.bin" . || mv "$1/${1/cpp/}" .
elif [ -d "$1.cpp" ]; then
    echo "$1/make"
    pushd "$1.cpp"
    make
    popd
    [ -e "$1.cpp/${1/cpp/}.bin" ] && mv "$1.cpp/${1/cpp/}.bin" . || mv "$1.cpp/${1/cpp/}" .
else
    echo "File $1 is not a file or folder"
    exit -1;
fi
