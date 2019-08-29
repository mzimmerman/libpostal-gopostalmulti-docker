#!/usr/bin/env bash
set -e

git clone https://github.com/msgpack/msgpack-c.git
cd msgpack-c
cmake .
make
make install

cd ..
gcc -o gopostalcmd gopostalcmd.c `pkg-config --cflags --libs libpostal`
