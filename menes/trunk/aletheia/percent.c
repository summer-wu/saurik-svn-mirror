#include "minimal/stdlib.h"
#include "minimal/mapping.h"

#include <string.h>

int main(int argc, const char *argv[]) {
    _assert(argc == 2);
    size_t sz;
    uint8_t *dt = map(argv[1], 0, _not(size_t), &sz, true);
    size_t zr = 0, of;
    for (of = 0; of != sz; ++of)
        if (dt[of] == 0x00)
            ++zr;
    printf("%f%%\n", 100.0 - zr * 100.0 / sz);
    return 0;
}
