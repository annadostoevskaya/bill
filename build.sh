#!/bin/bash

code="$PWD"
opts=-g
cd build > /dev/null
g++ $opts $code/sdl_bill.cpp -o sdl_bill.exe
cd $code > /dev/null
