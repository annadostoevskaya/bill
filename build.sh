#!/bin/bash

CC=clang
SRC="$PWD"
BUILD_DIR="build"
SDL_FLAGS=`sdl2-config --cflags --libs`
OPTIONS="-g -lm $SDL_FLAGS -D_CLI_ENABLED_ASSERTS -D_CLI_DEV_MODE"

if [ ! -d "./$BUILD_DIR" ]
then
    mkdir ./$BUILD_DIR
fi

pushd $BUILD_DIR > /dev/null
# TODO(annad): %SRC%/dev/ctime/ctime.exe -begin bill.ctm
$CC $OPTIONS $SRC/sdl_bill.cpp -o sdl_bill

popd > /dev/null
