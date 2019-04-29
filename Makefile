CC=gcc

all: shim.so example

shim.so:
	$(CC) -g -fPIC -shared -o shim.so shim.c -ldl

example:
	$(CC) -g -lpthread -o example example.c

clean:
	rm -rf example shim.so
