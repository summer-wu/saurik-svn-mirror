#include <stdio.h>

int main(int argc, char *argv[]) {
    int mask[4] = {0, 0, 0, 0};
    char *arg;
    for (arg = argv[1]; *arg; ++arg)
        mask[*arg / 32] |= 1 << *arg % 32;
    printf("0x%x, 0x%x, 0x%x, 0x%x\n", mask[0], mask[1], mask[2], mask[3]);
    return 0;
}
