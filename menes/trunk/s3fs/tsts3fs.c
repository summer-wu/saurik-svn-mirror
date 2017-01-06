#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define _syscall(expr) \
    do if ((expr) < 0) { \
        fprintf(stderr, "_syscall(%s): %d\n", #expr, errno); \
        exit(0); \
    } while (false)

int main(int argc, const char *argv[]) {
    int fd, sz;
    _syscall(fd = open(argv[1], O_RDONLY));
    printf("fd: %d\n", fd);
    _syscall(sz = write(fd, "i", 1));
    _syscall(close(fd));
    return 0;
}
