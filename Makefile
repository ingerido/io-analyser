CC=gcc

SYSCALL_INTERCEPT_DIR := $(abspath $(CURDIR)/syscall_intercept/install/)
INCLUDE=-I$(SYSCALL_INTERCEPT_DIR)/include
SHIMLDFLAGS=-L$(abspath $(SYSCALL_INTERCEPT_DIR)/lib) -Wl,-rpath=$(abspath $(SYSCALL_INTERCEPT_DIR)/lib) -lsyscall_intercept

all: shim.so example

shim.so:
	$(CC) $(INCLUDE) shim.c $(SHIMLDFLAGS) -fpic -shared -o shim.so

example:
	$(CC) -g -pthread -o example example.c

clean:
	rm -rf example shim.so #syscall_intercept
