#!/bin/bash

set -x

sudo apt-get install pkg-config libcapstone-dev

cd syscall_intercept
mkdir build
mkdir install
cd build
cmake -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_BUILD_TYPE=Release ..
make
make install

