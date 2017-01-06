#include <notify.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void check(int status) {
    char *error;
    switch (status) {
        case NOTIFY_STATUS_OK:
            return;
        case NOTIFY_STATUS_INVALID_NAME:
            error = "invalid name";
            break;
        case NOTIFY_STATUS_INVALID_TOKEN:
            error = "invalid token";
            break;
        case NOTIFY_STATUS_INVALID_PORT:
            error = "invalid port";
            break;
        case NOTIFY_STATUS_INVALID_FILE:
            error = "invalid file";
            break;
        case NOTIFY_STATUS_INVALID_SIGNAL:
            error = "invalid signal";
            break;
        case NOTIFY_STATUS_INVALID_REQUEST:
            error = "invalid request";
            break;
        case NOTIFY_STATUS_NOT_AUTHORIZED:
            error = "not authorized";
            break;
        case NOTIFY_STATUS_FAILED:
            error = "failed";
            break;
        default:
            error = "unknown";
            break;
    }

    fprintf(stderr, "error: %s\n", error);
    exit(1);
}

char *argv0_;

void usage(bool error) {
    fprintf(error ? stderr : stdout, "usage: %s <-p name|-r names...>\n", argv0_);
    exit(error ? 1 : 0);
}

int main(int argc, char *argv[]) {
    argv0_ = argv[0];

    if (
        argc < 2 ||
        argv[1][0] != '-'
    )
        usage(false);
    else switch (argv[1][1]) {
        case 'p':
            if (argc != 3)
                usage(true);
            check(notify_post(argv[2]));
        break;

        case 'r': {
            int fd = -1;
            for (int argi = 2; argi != argc; ++argi) {
                char *arga = argv[argi];
                int token;
                printf("%d\n", fd);
                check(notify_register_file_descriptor(arga, &fd, fd == -1 ? 0 : NOTIFY_REUSE, &token));
                check(notify_set_state(token, (uint64_t) (uintptr_t) arga));
            }

            size_t size = 0;
            int token;
            for (;;) {
                ssize_t result = read(fd, (char *) token + size, sizeof(token) - size);
                if (result == -1) {
                    perror(argv0_);
                    exit(1);
                } else if (size + result != sizeof(token)) {
                    size += result;
                } else {
                    uint64_t state;
                    check(notify_get_state(token, &state));
                    printf("%s\n", (char *) (uintptr_t) state);
                    size = 0;
                }
            }
        } break;

        default:
            usage(true);
    }

    return 0;
}
