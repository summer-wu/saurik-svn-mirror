#include <sys/types.h>

#include <sys/mman.h>
#include <sys/stat.h>

#include <netinet/in.h>

#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <string.h>

#define s_(b) do {int tm = (b); base += tm;} while (0)
#define s(b) base += b
#define g_(t) (* (t *) ((s(sizeof(t))) - sizeof(t)))
#define g4 ntohl(g_(uint32_t))
#define g2 ntohs(g_(uint16_t))
#define g1 g_(uint8_t)
#define p2(v) (* (uint16_t *) (base - sizeof(uint16_t)) = htons(v))

void inn(uint8_t *base, char **strings) {
    int i, end;
    uint16_t access;

    s(4);
    for (i = 0, end = g2; i != end; ++i) {
        s(6);
        access = g2;
        p2((access & ~0x0002 & ~0x0004 & ~0x0010) | 0x0001);
    }
}

void att(uint8_t **baser, char **strings) {
    uint8_t *base = *baser;
    int i, end;
    char *name;

    for (i = 0, end = g2; i != end; ++i) {
        name = strings[g2];
        if (strcmp(name, "InnerClasses") == 0)
            inn(base, strings);
        s_(g4);
    }

    *baser = base;
}

int main(int argc, char *argv[]) {
    int arg, fid;
    struct stat stat;
    uint8_t *base;
    uint16_t i, j, end;
    uint16_t access;
    uint32_t magic;
    char **strings;

    for (arg = 1; arg != argc; ++arg) {
        fid = open(argv[arg], O_RDWR);
        fstat(fid, &stat);
        base = mmap(NULL, stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fid, 0);

        magic = g4;
        if (magic != 0xcafebabe) {
            fprintf(stderr, "not a java class file: %s\n", argv[arg]);
            goto unmap;
        }

        s(4);

        end = g2;
        strings = malloc(end * sizeof(char *));
        strings[0] = NULL;

        for (i = 1; i != end; ++i)
            switch (g1) {
                case 1: /* Utf8 */
                    j = g2;
                    strings[i] = malloc(j + 1);
                    memcpy(strings[i], base, j);
                    strings[i][j] = '\0';
                    s_(j);
                    break;

                case 3: /* Integer */
                case 4: /* Float */
                    s(4);
                    break;

                case 5: /* Long */
                case 6: /* Double */
                    s(8);
                    break;

                case 7: /* Classs */
                    s(2);
                    break;

                case 8: /* String */
                    s(2);
                    break;

                case 9: /* Fieldref */
                case 10: /* Methodref */
                case 11: /* InterfaceMethodref */
                    s(4);
                    break;

                case 12: /* NameAndType */
                    s(4);
                    break;

                default:
                    exit(-1);
            }

        access = g2;
        p2((access & ~0x0010) | 0x0001);

        s(4);
        s_(g2 * 2);

        for (j = 0; j != 2; ++j)
            for (i = 0, end = g2; i != end; ++i) {
                access = g2;
                p2((access & ~0x0002 & ~0x0004 & ~0x0010) | 0x0001);
                s(4);
                att(&base, strings);
            }

        att(&base, strings);

    unmap:
        munmap(base, stat.st_size);
        close(fid);
    }

    return 0;
}
