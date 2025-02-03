#!/bin/sh
cppcheck -q --enable=all --inconclusive -I sysroot/usr/include ./
