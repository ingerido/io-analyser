# io-analyser
io-analyser is a tiny shared library that gives you statistics of read/write system call in an application.

This repo is also with an example.

## make
> git clone https://github.com/pmem/syscall_intercept.git
> ./makeshim.sh

## run

> LD_PRELOAD=./shim.so ./example

## result
The result is in io_syscall_record.txt, each row contains following information: filename | PID | TID | syscall | IO Size | Offset 
