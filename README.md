# io-analyser
io-analyser is a tiny shared library that gives you statistics of read/write system call in an application.

This repo also shipped with an example.

## run

> LD_PRELOAD=./shim.so example

## result
The result is in io_syscall_record.txt, each row contains following information: filename PID TID syscall
