#!/bin/sh

export CXX=/usr/bin/clang++
export CC=/usr/bin/clang

cmake .
cpus=$(grep -c ^processor /proc/cpuinfo)
if (make -j $cpus) then
    ./maximumparsimony
fi
