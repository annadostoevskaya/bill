#!/bin/bash

SRC="$PWD"
BUILD_DIR="build"

if [ ! -d "./$BUILD_DIR" ]
then
    mkdir ./$BUILD_DIR
fi

pushd $BUILD_DIR > /dev/null
$SRC/dev/ctime/ctime -begin bill.ctm
clang++ -g -Ofast -c -std=c++11 -msse4.1 $SRC/bill_renderer_software.cpp -o bill_renderer_software.o
clang++ -g -std=c++11 -D _ENABLED_ASSERT -D _CLI_DEV_MODE `sdl2-config --cflags --libs` bill_renderer_software.o $SRC/sdl_bill.cpp -o sdl_bill
$SRC/dev/ctime/ctime -end bill.ctm $? 
popd > /dev/null
