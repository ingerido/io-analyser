#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <linux/limits.h>

#include <libsyscall_intercept_hook_point.h>

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

int shim_do_read(int fd, void *buf, size_t count, size_t* result)
{
  size_t ret;

  if (check_mlfs_fd(fd)) {
    ret = mlfs_posix_read(get_mlfs_fd(fd), buf, count);
    syscall_trace(__func__, ret, 3, fd, buf, count);

    *result = ret;
    return 0;
  } else {
    return 1;
  }
}

int shim_do_pread64(int fd, void *buf, size_t count, loff_t off, size_t* result)
{
  size_t ret;

  if (check_mlfs_fd(fd)) {
    ret = mlfs_posix_pread64(get_mlfs_fd(fd), buf, count, off);
    syscall_trace(__func__, ret, 4, fd, buf, count, off);

    *result = ret;
    return 0;
  } else {
    return 1;
  }
}

int shim_do_write(int fd, void *buf, size_t count, size_t* result)
{
  size_t ret;

  if (check_mlfs_fd(fd)) {
    ret = mlfs_posix_write(get_mlfs_fd(fd), buf, count);
    syscall_trace(__func__, ret, 3, fd, buf, count);

    *result = ret;
    return 0;
  } else {
    return 1;
  }

}

int shim_do_pwrite64(int fd, void *buf, size_t count, loff_t off, size_t* result)
{
  size_t ret;

  if (check_mlfs_fd(fd)) {
    /*
    ret = mlfs_posix_pwrite64(get_mlfs_fd(fd), buf, count, off);
    syscall_trace(__func__, ret, 4, fd, buf, count, off);

    *result = ret;
    */
    printf("%s: does not support yet\n", __func__);
    exit(-1);
  } else {
    return 1;
  }

}


static int hook(long syscall_number,
     long arg0, long arg1,
     long arg2, long arg3,
     long arg4, long arg5,
     long *result) {
  switch (syscall_number) {
    /*case SYS_open: return shim_do_open((char*)arg0, (int)arg1, (mode_t)arg2, (int*)result);
    case SYS_openat: return shim_do_openat((int)arg0, (const char*)arg1, (int)arg2, (mode_t)arg3, (int*)result);
    case SYS_creat: return shim_do_creat((char*)arg0, (mode_t)arg1, (int*)result);*/
    case SYS_read: return shim_do_read((int)arg0, (void*)arg1, (size_t)arg2, (size_t*)result);
    case SYS_pread64: return shim_do_pread64((int)arg0, (void*)arg1, (size_t)arg2, (loff_t)arg3, (size_t*)result);
    case SYS_write: return shim_do_write((int)arg0, (void*)arg1, (size_t)arg2, (size_t*)result);
    case SYS_pwrite64: return shim_do_pwrite64((int)arg0, (void*)arg1, (size_t)arg2, (loff_t)arg3, (size_t*)result);
    /*case SYS_close: return shim_do_close((int)arg0, (int*)result);
    case SYS_lseek: return shim_do_lseek((int)arg0, (off_t)arg1, (int)arg2, (int*)result);
    case SYS_mkdir: return shim_do_mkdir((void*)arg0, (mode_t)arg1, (int*)result);
    case SYS_rmdir: return shim_do_rmdir((const char*)arg0, (int*)result);
    case SYS_rename: return shim_do_rename((char*)arg0, (char*)arg1, (int*)result);
    case SYS_fallocate: return shim_do_fallocate((int)arg0, (int)arg1, (off_t)arg2, (off_t)arg3, (int*)result);
    case SYS_stat: return shim_do_stat((const char*)arg0, (struct stat*)arg1, (int*)result);
    case SYS_lstat: return shim_do_lstat((const char*)arg0, (struct stat*)arg1, (int*)result);
    case SYS_fstat: return shim_do_fstat((int)arg0, (struct stat*)arg1, (int*)result);
    case SYS_truncate: return shim_do_truncate((const char*)arg0, (off_t)arg1, (int*)result);
    case SYS_ftruncate: return shim_do_ftruncate((int)arg0, (off_t)arg1, (int*)result);
    case SYS_unlink: return shim_do_unlink((const char*)arg0, (int*)result);
    case SYS_symlink: return shim_do_symlink((const char*)arg0, (const char*)arg1, (int*)result);
    case SYS_access: return shim_do_access((const char*)arg0, (int)arg1, (int*)result);
    case SYS_fsync: return shim_do_fsync((int)arg0, (int*)result);
    case SYS_fdatasync: return shim_do_fdatasync((int)arg0, (int*)result);
    case SYS_sync: return shim_do_sync((int*)result);
    case SYS_fcntl: return shim_do_fcntl((int)arg0, (int)arg1, (void*)arg2, (int*)result);
    case SYS_mmap: return shim_do_mmap((void*)arg0, (size_t)arg1, (int)arg2, (int)arg3, (int)arg4, (off_t)arg5, (void**)result);
    case SYS_munmap: return shim_do_munmap((void*)arg0, (size_t)arg1, (int*)result);
    case SYS_getdents: return shim_do_getdents((int)arg0, (struct linux_dirent*)arg1, (size_t)arg2, (size_t*)result);
    case SYS_getdents64: return shim_do_getdents64((int)arg0, (struct linux_dirent64*)arg1, (size_t)arg2, (size_t*)result);*/
  }
  return 1;
}

static __attribute__((constructor)) void init(void)
{
  // Set up the callback function
  intercept_hook_point = hook;
}
