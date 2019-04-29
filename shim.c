#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <linux/limits.h>

#define RECORD "io_syscall_record.txt"

static int initialized = 0;
static FILE *rec_file = NULL;

// Only works on Linux
void record_syscall(int fd, char *ope) {
    char fd_path[PATH_MAX];
	char file_path[PATH_MAX];
	char record[512];
	int file_path_len;
	pid_t pid, tid;

	if (initialized == 0) {
		if ((rec_file = fopen(RECORD, "a")) == NULL) {
			exit(0);
		}
		initialized = 1;
	}

	/* Get I/O file path */
    sprintf(fd_path, "/proc/self/fd/%d", fd);
    if ((file_path_len = readlink(fd_path, file_path, PATH_MAX-1)) < 0) {
        return;
	}
    file_path[file_path_len] = '\0';	

	/* Get I/O TID PID */
	pid = getpid();
	tid = syscall(SYS_gettid);

	/* Insert this system call record to file */
	sprintf(record, "%s, %d, %d, %s", file_path, pid, tid, ope);
	//printf("%s\n", record);
	fprintf(rec_file,"%s\n", record); 

}

ssize_t read(int fd, void *buf, size_t count) {

    size_t (*lread)(int, void*, size_t) = dlsym(RTLD_NEXT, "read");

	record_syscall(fd, "read");

    return lread(fd, buf, count);
}


ssize_t pread(int fd, void *buf, size_t count, off_t offset) {

    size_t (*lpread)(int, void*, size_t, off_t) = dlsym(RTLD_NEXT, "pread");

	record_syscall(fd, "pread");

    return lpread(fd, buf, count, offset);
}


ssize_t write(int fd, const void *buf, size_t count) {

    size_t (*lwrite)(int, const void*, size_t) = dlsym(RTLD_NEXT, "write");

	record_syscall(fd, "write");

    return lwrite(fd, buf, count);
}


ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) {

    size_t (*lpwrite)(int, const void*, size_t, off_t) = dlsym(RTLD_NEXT, "pwrite");

	record_syscall(fd, "pwrite");

    return lpwrite(fd, buf, count, offset);
}
