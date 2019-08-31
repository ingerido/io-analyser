#!/bin/bash

set -x

sudo apt-get install pkg-config libcapstone-dev

if [ ! -d syscall_intercept ]; then
	git clone https://github.com/pmem/syscall_intercept.git
fi

cd syscall_intercept
mkdir build
mkdir install
cd build
cmake -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_BUILD_TYPE=Release ..
make
make install

