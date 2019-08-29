#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#define ITER 5
#define FILENAME "testfile"

static int fd = -1;

void *read_thread(void *arg) {
	char buf[1024];

	for (int i = 0; i < ITER; ++i)
		read(fd, buf, 1024);

	pread(fd, buf, 1024, 1024);

	return NULL;
}

void *write_thread(void *arg) {
	char buf[4096];

	memset(buf, 0x31, 4096);

	write(fd, buf, 4096);
	
	pwrite(fd, buf, 1024, 4096);
}

int main(int argc, char **argv) {
	pthread_t r1, r2, w1, w2;

	if ((fd = open(FILENAME, O_RDWR | O_CREAT, 0666)) < 0) {
		exit(0);
	}

	pthread_create(&r1, NULL, &read_thread, NULL);
	pthread_create(&r2, NULL, &read_thread, NULL);
	pthread_create(&w1, NULL, &write_thread, NULL);
	pthread_create(&w2, NULL, &write_thread, NULL);		

	pthread_join(r1, NULL);
	pthread_join(r2, NULL);
	pthread_join(w1, NULL);
	pthread_join(w2, NULL);

    return 0;
}
