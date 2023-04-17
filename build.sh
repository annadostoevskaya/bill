#!/bin/bash

CC=clang
SRC="$PWD"
BUILD_DIR="build"
SDL_FLAGS=`sdl2-config --cflags --libs`
OPTIONS="-msse4.1 -g -lm $SDL_FLAGS -D_ENABLED_ASSERT -D_CLI_DEV_MODE"

if [ ! -d "./$BUILD_DIR" ]
then
    mkdir ./$BUILD_DIR
fi

pushd $BUILD_DIR > /dev/null
$SRC/dev/ctime/ctime -begin bill.ctm
$CC $OPTIONS $SRC/sdl_bill.cpp -o sdl_bill
$SRC/dev/ctime/ctime -end bill.ctm $? 
popd > /dev/null
