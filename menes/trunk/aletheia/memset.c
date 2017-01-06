#include "minimal/stdlib.h"
#include "minimal/mapping.h"

#include <string.h>

int main(int argc, const char *argv[]) {
    _assert(argc == 5);
    size_t sz;
    uint8_t *dt = map(argv[1], 0, _not(size_t), &sz, false);
    size_t bg = strtoul(argv[2], NULL, 0);
    size_t ed = strtoul(argv[3], NULL, 0);
    uint8_t vl = strtoul(argv[4], NULL, 0);
    memset(dt + bg, vl, ed - bg);
    return 0;
}
