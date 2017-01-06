#include "minimal/stdlib.h"
#include "minimal/mapping.h"

int main(int argc, const char *argv[]) {
    _assert(argc == 3);
    size_t sz;
    uint8_t *dt = map(argv[1], 0, _not(size_t), &sz, true);
    uint32_t vl = strtoul(argv[2], NULL, 0);
    sz &= ~3;
    size_t of;
    for (of = 0; of != sz; of += 4)
        if (* (uint32_t *) (dt + of) == vl)
            printf("%s: 0x%.6zx\n", argv[1], of);
    return 0;
}
