#include <errno.h>
#include <mach/mach_init.h>
#include <mach/vm_map.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/signal.h>
#include <stdlib.h>

#ifdef __arm__
static int test() {
    __asm (".rept 1024; nop; .endr");
    return 0x123;
}
#endif

static mach_port_t task;
static size_t page;

static void look(void *base, const char **message, vm_prot_t *cur, vm_prot_t *max, kern_return_t *ret) {
    if (base == MAP_FAILED) {
        *message = "mmap fail!";
        *cur = 0;
        *max = 0;
        *ret = 0;
    } else {
        vm_address_t address = (vm_address_t) base;
        struct vm_region_basic_info_64 info;
        vm_size_t size;
        mach_msg_type_number_t count = VM_REGION_BASIC_INFO_COUNT_64;
        mach_port_t object;

        *ret = vm_region(task, &address, &size, VM_REGION_BASIC_INFO, (vm_region_info_t) &info, &count, &object);
        if (*ret == 0) {
            *message = "success!";
            *cur = info.protection;
            *max = info.max_protection;
        } else {
            *message = "vm_region fail!";
            *cur = 0;
            *max = 0;
        }
    }
}

static void skim(vm_prot_t prot, char name[4]) {
    name[0] = (prot & VM_PROT_READ) == 0 ? '-' : 'r';
    name[1] = (prot & VM_PROT_WRITE) == 0 ? '-' : 'w';
    name[2] = (prot & VM_PROT_EXECUTE) == 0 ? '-' : 'x';
    name[3] = '\0';
}

static void *alloc(
    bool write, bool exec, bool *fail,
    const char *v1cur, const char *v1max, int v1err,
    const char *v2cur, const char *v2max, int v2err
) {
    char v0ask[4] = {'r', write ? 'w' : '-', exec ? 'x' : '-', '\0'};

    int flags = PROT_READ;
    if (write) flags |= PROT_WRITE;
    if (exec) flags |= PROT_EXEC;

    errno = 0;

    void *base = mmap(NULL, page, flags, MAP_ANON | MAP_PRIVATE, -1, 0);

    const char *message;
    vm_prot_t cur, max;
    kern_return_t ret;
    look(base, &message, &cur, &max, &ret);

    char v0cur[4], v0max[4];
    skim(cur, v0cur);
    skim(max, v0max);

    vm_prot_t ask = VM_PROT_READ;
    if (write) ask |= VM_PROT_WRITE;
    if (exec) ask |= VM_PROT_EXECUTE;
    *fail = ask != cur;

    printf("%s? =%s<%s:%2d (=%s<%s:%2d) (=%s<%s:%2d) :(%d) %s\n", v0ask, v0cur, v0max, errno, v1cur, v1max, v1err, v2cur, v2max, v2err, ret, message == NULL ? "" : message);
    return base;
}

static void flip(
    void *base, bool flip, bool execute,
    const char *v2cur, const char *v2max, int v2err
) {
    errno = 0;
    mprotect(base, page, PROT_READ | (execute ? PROT_EXEC : PROT_WRITE));

    const char *message;
    vm_prot_t cur, max;
    kern_return_t ret;
    look(base, &message, &cur, &max, &ret);

    char v0cur[4], v0max[4];
    skim(cur, v0cur);
    skim(max, v0max);

    printf("%c%s =%s<%s:%2d (=%s<rwx:%2d) (=%s<%s:%2d) :(%d) %s\n", flip ? '>' : '<', execute ? "r-x" : "rw-", v0cur, v0max, errno, execute ? "r-x" : "rw-", 0, v2cur, v2max, v2err, ret, message == NULL ? "" : message);
}

int main() {
    task = mach_task_self();
    page = vm_page_size;

    printf("ask? =cur<max:er (MacOS10.5.5) (iPwnedOS2.x)\n");

    bool fail;

    void *rw_ = alloc(true, false, &fail, "rw-", "rwx", 0, "rw-", "rwx", 0);
    void *r_x = alloc(false, true, &fail, "r-x", "rwx", 0, "r-x", "r-x", 0);
    void *rwx = alloc(true, true, &fail, "rwx", "rwx", 0, "rw-", "rwx", 0);

    flip(rw_, true, true, "r-x", "rwx", 0);
    flip(rw_, false, false, "rw-", "rwx", 0);
    flip(r_x, true, false, "r-x", "r-x", 13);

    kern_return_t ret;

    void *base = mmap(NULL, page, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    if (rw_ == MAP_FAILED)
        printf("frowny /pants/\n");
    else {
        errno = 0;
        mprotect(base, page, PROT_READ | PROT_WRITE | PROT_EXEC);

        const char *message;
        vm_prot_t cur, max;
        look(base, &message, &cur, &max, &ret);

        char v0cur[4], v0max[4];
        skim(cur, v0cur);
        skim(max, v0max);

        printf("!rwx =%s<%s:%2d (=rwx<rwx: 0) (=rw-<rwx: 0) :(%d) %s\n", v0cur, v0max, errno, ret, message == NULL ? "" : message);
    }

    base = mmap(NULL, page, PROT_READ | PROT_EXEC, MAP_ANON | MAP_PRIVATE, -1, 0);
    if (rw_ == MAP_FAILED)
        printf("frowny /pants/\n");
    else {
        errno = 0;
        vm_protect(task, base, page, FALSE, VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE | VM_PROT_COPY);

        const char *message;
        vm_prot_t cur, max;
        look(base, &message, &cur, &max, &ret);

        char v0cur[4], v0max[4];
        skim(cur, v0cur);
        skim(max, v0max);

        printf("#rwx =%s<%s:%2d (=rwx<rwx: 0) (=rw-<rwx: 0) :(%d) %s\n", v0cur, v0max, errno, ret, message == NULL ? "" : message);
    }

#ifdef __arm__
    if (fail)
        printf("$rwx forbidden\n");
    else {
        printf("$rwx ");
        fflush(stdout);

        uint32_t *code = (uint32_t *) rwx;
        code[0] = 0xe3a00e21;

        printf(".");
        fflush(stdout);

        code[1] = 0xe12fff1e;

        printf(".");
        fflush(stdout);

        printf(".");
        fflush(stdout);

        int r0 = (* (int (*)(void)) code)();

        printf(" %s\n", r0 == 0x210 ? "success" : "failure");
    }

    fflush(stdout);

    uintptr_t diff = abs((uintptr_t) main - (uintptr_t) test);
    if (diff < page + 8)
        printf("$r<> unsafe (%u)\n", diff);
    else {
        uintptr_t addr = (uintptr_t) test / page * page;
        if (page - ((uintptr_t) test - addr) < 8)
            page *= 2;

        const char *message;
        vm_prot_t cur, max;
        char v0cur[4], v0max[4];

        look(addr, &message, &cur, &max, &ret);
        skim(cur, v0cur);
        skim(max, v0max);
        printf("#rwx =%s<%s:%2d (=rwx<rwx: 0) (=rw-<rwx: 0) :(%d) %s\n", v0cur, v0max, errno, ret, message == NULL ? "" : message);

        ret = vm_protect(task, addr, page, FALSE, VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE | VM_PROT_COPY);

        look(addr, &message, &cur, &max, &ret);
        skim(cur, v0cur);
        skim(max, v0max);
        printf("#rwx =%s<%s:%2d (=rwx<rwx: 0) (=rw-<rwx: 0) :(%d) %s\n", v0cur, v0max, errno, ret, message == NULL ? "" : message);

        ret = vm_protect(task, addr, page, FALSE, VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE | VM_PROT_COPY);

        look(addr, &message, &cur, &max, &ret);
        skim(cur, v0cur);
        skim(max, v0max);
        printf("#rwx =%s<%s:%2d (=rwx<rwx: 0) (=rw-<rwx: 0) :(%d) %s\n", v0cur, v0max, errno, ret, message == NULL ? "" : message);

        ret = mprotect(addr, page, PROT_READ | PROT_WRITE | PROT_EXEC);

        look(addr, &message, &cur, &max, &ret);
        skim(cur, v0cur);
        skim(max, v0max);
        printf("#rwx =%s<%s:%2d (=rwx<rwx: 0) (=rw-<rwx: 0) :(%d) %s\n", v0cur, v0max, errno, ret, message == NULL ? "" : message);

        ret = vm_protect(task, addr, page, FALSE, VM_PROT_READ | VM_PROT_WRITE | VM_PROT_COPY);

        if (ret != 0)
            printf("$r<> error vm_protect(rw_|c)=%d\n", ret);
        else {
            printf("$r<> ");
            fflush(stdout);

            uint32_t *code = (uint32_t *) test;
            code[0] = 0xe3a00e21;

            printf(".");
            fflush(stdout);

            code[1] = 0xe12fff1e;

            printf(".");
            fflush(stdout);

            printf(".");
            fflush(stdout);

            ret = vm_protect(task, addr, page, FALSE, VM_PROT_READ | VM_PROT_EXECUTE);
            if (ret != 0)
                printf(" error vm_protect(r_x|0)=%d\n", ret);
            else {
                int r0 = test();
                printf(" %s\n", r0 == 0x210 ? "success" : "failure");
            }
        }
    }

    fflush(stdout);
#endif

    return 0;
}
