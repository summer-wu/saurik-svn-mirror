#define _GNU_SOURCE

#include "minimal/stdlib.h"
#include "minimal/mapping.h"

#include <string.h>

static const char *cc_[] =
    {"eq","ne","cs","cc","mi","pl","vs","vc",
     "hi","ls","ge","lt","gt","le",""  ,NULL};

static const char *rg_[] =
    {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
     "r8", "r9", "r10", "r11", "r12", "sp", "lr", "pc"};

static const char *sh_[] =
    {"lsl", "lsr", "asr", "ror"};

char Characterize(uint8_t value) {
    return value < 0x20 || value >= 0x7f ? '.' : (char) value;
}

typedef uint32_t pointer_t;

struct Symbol_ {
    struct Symbol_ *nx;
    pointer_t of;
    const char *nm;
};

struct Context_ {
    const uint8_t *dt;
    size_t sz;
    const uint8_t *fm;
    pointer_t bs;
    struct Symbol_ *sy;
};

struct Formatter_ {
    struct Formatter_ *nx;
    uint8_t id;
    const char *nm;
    size_t (*fm)(struct Context_ *, size_t);
    char *(*sy)(struct Context_ *, size_t, unsigned);
    bool (*pf)(struct Context_ *, size_t);
    void (*fl)(struct Context_ *, size_t, size_t);
} *fm_;

pointer_t Adjust(struct Context_ *cx, size_t ix) {
    return ix + cx->bs;
}

bool Locate(struct Context_ *cx, pointer_t of, size_t *ix) {
    size_t ip = of - cx->bs;
    if (ip >= cx->sz)
        return false;
    else if (ix != NULL)
        *ix = ip;
    return true;
}

char *Find(struct Context_ *cx, pointer_t of) {
    struct Symbol_ *sy;
    for (sy = cx->sy; sy != NULL; sy = sy->nx)
        if (sy->of == (of & ~0x1))
            return strdup(sy->nm);
    return NULL;
}

char *Lookup(struct Context_ *cx, pointer_t of, unsigned dp);

char *UInt1_Symbol(struct Context_ *cx, size_t ix, unsigned dp) {
    uint8_t vl = * (const uint8_t *) (cx->dt + ix);

    char *sy;
    asprintf(&sy, "0x%.2x (%u)", vl, vl);
    return sy;
}

char *UInt2_Symbol(struct Context_ *cx, size_t ix, unsigned dp) {
    uint16_t vl = * (const uint16_t *) (cx->dt + ix);

    char *sy;
    asprintf(&sy, "0x%.4x (%u)", vl, vl);
    return sy;
}

char *UInt4_Symbol(struct Context_ *cx, size_t ix, unsigned dp) {
    uint32_t vl = * (const uint32_t *) (cx->dt + ix);

    char *sy;
    asprintf(&sy, "0x%.8x (%u)", vl, vl);
    return sy;
}

char *Zeros_Symbol(struct Context_ *cx, size_t ix, unsigned dp) {
    return strdup("<00...>");
}

char *Ehffs_Symbol(struct Context_ *cx, size_t ix, unsigned dp) {
    return strdup("<ff...>");
}

char *Magic_Symbol(struct Context_ *cx, size_t ix, unsigned dp) {
    uint32_t vl = * (const uint32_t *) (cx->dt + ix);

    char *sy;
    asprintf(&sy, "'%c%c%c%c'/'%c%c%c%c'/%.8x",
        Characterize(vl >> 24 & 0xff),
        Characterize(vl >> 16 & 0xff),
        Characterize(vl >>  8 & 0xff),
        Characterize(vl >>  0 & 0xff),
        Characterize(vl >>  0 & 0xff),
        Characterize(vl >>  8 & 0xff),
        Characterize(vl >> 16 & 0xff),
        Characterize(vl >> 24 & 0xff),
        vl
    );
    return sy;
}

void Read(char *sy, const uint8_t *dt, size_t sz) {
    size_t ps = 0;
    sy[ps++] = '"';

    size_t pn;
    for (pn = 0; pn != sz; ++pn) {
        uint8_t vl = dt[pn];
        switch (vl) {
            case '\n':
                sy[ps++] = '\\';
                sy[ps++] = 'n';
            break;

            case '\t':
                sy[ps++] = '\\';
                sy[ps++] = 't';
            break;

            case '\r':
                sy[ps++] = '\\';
                sy[ps++] = 'r';
            break;

            default:
                sy[ps++] = vl;
            break;
        }
    }

    sy[ps++] = '"';
    sy[ps++] = '\0';
}

char *Table_Symbol(struct Context_ *cx, size_t ix, unsigned dp) {
    size_t sz = strlen((char *) (cx->dt + ix));
    char *sy = malloc(sz * 2 + 3);
    Read(sy, cx->dt + ix, sz);
    return sy;
}

void Print(const uint8_t *dt, size_t sz) {
    printf("\"");

    size_t ix;

    for (ix = 0; ix != sz; ++ix)
        switch (dt[ix]) {
            case '\n': printf("\\n"); break;
            case '\t': printf("\\t"); break;
            case '\r': printf("\\r"); break;

            default:
                printf("%c", dt[ix]);
            break;
        }

    printf("\"");
}

size_t Chars_Format(struct Context_ *cx, size_t ix) {
    size_t sz;
    for (sz = 0; ix + sz != cx->sz && cx->fm[ix + sz] == 9; ++sz);
    printf("..            == (%zu) ", sz);
    Print(cx->dt + ix, sz);
    return sz;
}

char *Chars_Symbol(struct Context_ *cx, size_t ix, unsigned dp) {
    size_t sz;
    for (sz = 0; ix + sz != cx->sz && cx->fm[ix + sz] == 9; ++sz);
    char *sy = malloc(sz * 2 + 3);
    Read(sy, cx->dt + ix, sz);
    return sy;
}

char *Point_Symbol(struct Context_ *cx, size_t ix, unsigned dp) {
    if (dp == 3)
        return strdup("...");

    uint32_t vl = * (const uint32_t *) (cx->dt + ix);

    char *sv = Lookup(cx, vl, dp + 1);
    if (sv == NULL)
        return UInt4_Symbol(cx, ix, dp);

    char *sy;
    asprintf(&sy, "0x%.8x:&%s", vl, sv);
    free(sv);
    return sy;
}

size_t UInt1_Format(struct Context_ *cx, size_t of) {
    uint8_t vl = * (const uint8_t *) (cx->dt + of);

    printf("%.2x            ",
        cx->dt[of + 0]
    );

    printf("== ");

    printf("0x%.2x", vl);

    printf("         %c", Characterize(vl));
    printf("                       ; (%u)", vl);

    return 1;
}

size_t UInt2_Format(struct Context_ *cx, size_t of) {
    uint16_t vl = * (const uint16_t *) (cx->dt + of);

    printf("%.2x %.2x         ",
        cx->dt[of + 0],
        cx->dt[of + 1]
    );

    printf("== ");

    printf("0x%.4x", vl);

    printf("       %c%c       %c%c",
        Characterize(vl >>  8 & 0xff),
        Characterize(vl >>  0 & 0xff),
        Characterize(vl >>  0 & 0xff),
        Characterize(vl >>  8 & 0xff)
    );

    printf("             ; (%u)", vl);

    return 2;
}

size_t UInt4_Format(struct Context_ *cx, size_t of) {
    uint32_t vl = * (const uint32_t *) (cx->dt + of);
    const char *sy = Lookup(cx, vl, 0);

    printf("%.2x %.2x %.2x %.2x   ",
        cx->dt[of + 0],
        cx->dt[of + 1],
        cx->dt[of + 2],
        cx->dt[of + 3]
    );

    printf("== ");

    printf("0x%.8x", vl);

    printf("   %c%c%c%c   %c%c%c%c ",
        Characterize(vl >> 24 & 0xff),
        Characterize(vl >> 16 & 0xff),
        Characterize(vl >>  8 & 0xff),
        Characterize(vl >>  0 & 0xff),
        Characterize(vl >>  0 & 0xff),
        Characterize(vl >>  8 & 0xff),
        Characterize(vl >> 16 & 0xff),
        Characterize(vl >> 24 & 0xff)
    );

    printf("  %.4x %.4x",
        vl >>  0 & 0xffff,
        vl >> 16 & 0xffff
    );

    if (sy == NULL)
        printf(" ; (%u)", vl);
    else
        printf(" ; &%s", sy);

    return 4;
}

size_t Point_Format(struct Context_ *cx, size_t of) {
    uint32_t vl = * (const uint32_t *) (cx->dt + of);
    const char *sy = Lookup(cx, vl, 0);

    printf("%.2x %.2x %.2x %.2x   ",
        cx->dt[of + 0],
        cx->dt[of + 1],
        cx->dt[of + 2],
        cx->dt[of + 3]
    );

    printf("-> 0x%.8x",
        vl
    );

    if (sy != NULL)
        printf(" ; %s", sy);

    return 4;
}

size_t Table_Format(struct Context_ *cx, size_t ix) {
    size_t sz = strlen((char *) (cx->dt + ix));
    printf("..            == (%zu) ", sz);
    Print(cx->dt + ix, sz);
    while (cx->dt[ix + ++sz] == '\0');
    return sz;
}

size_t Arm11_Format(struct Context_ *cx, size_t of) {
    uint32_t ic = * (const uint32_t *) (cx->dt + of);
    uint32_t pc = Adjust(cx, of) + 8;

    const char *sy = NULL;

    printf("%.2x %.2x %.2x %.2x   ",
        cx->dt[of + 0],
        cx->dt[of + 1],
        cx->dt[of + 2],
        cx->dt[of + 3]
    );

    if (false) {
    } else if ((ic & 0x0e000010) == 0x00000000 && (ic & 0xf0000000) != 0xf0000000 && (ic & 0x01900000) != 0x01000000) { /* {{{ data processing immediate shift */
        union {
            uint32_t vl;

            struct {
                uint32_t rm : 4;
                uint32_t  : 1;
                uint32_t shift : 2;
                uint32_t shiftamount : 5;
                uint32_t rd : 4;
                uint32_t rn : 4;
                uint32_t s : 1;
                uint32_t opcode : 4;
                uint32_t : 3;
                uint32_t cond : 4;
            };
        } id = {ic};

        static const char *oc_[] =
            {"and", "eor", "sub", "rsb", "add", "adc", "sbc", "rsc",
             "tst", "teq", "cmp", "cmn", "orr", "mov", "bic", "mvn"};

        printf(oc_[id.opcode]);
        printf("%s", cc_[id.cond]);
        if (id.s != 0)
            printf("s");
        if ((id.opcode & 0xc) != 0x8)
            printf(" %s,", rg_[id.rd]);
        if ((id.opcode & 0xd) != 0xd)
            printf(" %s,", rg_[id.rn]);
        if (id.rm == 15)
            printf(" 0x%.8x", pc);
        else
            printf(" %s", rg_[id.rm]);
        if (id.shift == 0x3 && id.shiftamount == 0)
            printf(", rrx");
        else if (id.shift != 0x0 || id.shiftamount != 0)
            printf(", %s %u", sh_[id.shift], id.shift != 0x0 && id.shiftamount == 0 ? 32 : id.shiftamount);
    /* }}} */
    } else if ((ic & 0x0fb000f0) == 0x01000000 && (ic & 0xf0000000) != 0xf0000000) { /* {{{ move status register to register */
        union {
            uint32_t vl;

            struct {
                uint32_t : 12;
                uint32_t rd : 4;
                uint32_t : 6;
                uint32_t r : 1;
                uint32_t : 5;
                uint32_t cond : 4;
            };
        } id = {ic};

        printf("mr%s %s, %cpsr", cc_[id.cond], rg_[id.rd], id.r == 0 ? 'c' : 's');
    /* }}} */
    } else if ((ic & 0x0fb000f0) == 0x01200000 && (ic & 0xf0000000) != 0xf0000000) { /* {{{ move register to status register */
        union {
            uint32_t vl;

            struct {
                uint32_t rm : 4;
                uint32_t : 12;
                uint32_t mask : 4;
                uint32_t : 2;
                uint32_t r : 1;
                uint32_t : 5;
                uint32_t cond : 4;
            };
        } id = {ic};

        printf("msr%s %cpsr_", cc_[id.cond], id.r == 0 ? 'c' : 's');
        if ((id.mask & 0x1) != 0)
            printf("c");
        if ((id.mask & 0x2) != 0)
            printf("x");
        if ((id.mask & 0x4) != 0)
            printf("s");
        if ((id.mask & 0x8) != 0)
            printf("f");
        printf(", %s", rg_[id.rm]);
    /* }}} */
    } else if ((ic & 0x0ff000f0) == 0x01200010 && (ic & 0xf0000000) != 0xf0000000) { /* {{{ branch/exchange instruction set */
        union {
            uint32_t vl;

            struct {
                uint32_t rm : 4;
                uint32_t : 24;
                uint32_t cond : 4;
            };
        } id = {ic};

        printf("bx%s %s", cc_[id.cond], rg_[id.rm]);
    /* }}} */
    } else if ((ic & 0x0ff000f0) == 0x01600010 && (ic & 0xf0000000) != 0xf0000000) { /* {{{ count leading zeros */
        union {
            uint32_t vl;

            struct {
                uint32_t rm : 4;
                uint32_t : 8;
                uint32_t rd : 4;
                uint32_t : 24;
                uint32_t cond : 4;
            };
        } id = {ic};

        printf("clz%s %s, %s", cc_[id.cond], rg_[id.rd], rg_[id.rm]);
    /* }}} */
    } else if ((ic & 0x0ff000f0) == 0x01200030 && (ic & 0xf0000000) != 0xf0000000) { /* {{{ branch and link/exchange instruction set */
        union {
            uint32_t vl;

            struct {
                uint32_t rm : 4;
                uint32_t : 24;
                uint32_t cond : 4;
            };
        } id = {ic};

        printf("blx%s %s", cc_[id.cond], rg_[id.rm]);
    /* }}} */
    } else if ((ic & 0x0f9000f0) == 0x01000050 && (ic & 0xf0000000) != 0xf0000000) { /* {{{ enhanced dsp add/subtracts */
        union {
            uint32_t vl;

            struct {
                uint32_t rm : 4;
                uint32_t : 8;
                uint32_t rd : 4;
                uint32_t rn : 4;
                uint32_t : 1;
                uint32_t op : 2;
                uint32_t : 5;
                uint32_t cond : 4;
            };
        } id = {ic};

        static const char *oc_[] =
            {"add", "sub", "dadd", "dsub"};

        printf("q%s%s %s, %s, %s", oc_[id.op], cc_[id.cond], rg_[id.rd], rg_[id.rm], rg_[id.rn]);
    /* }}} */
    } else if ((ic & 0x0ff000f0) == 0x01200070 && (ic & 0xf0000000) != 0xf0000000) { /* {{{ software breakpoint */
        union {
            uint32_t vl;

            struct {
                uint32_t rm : 4;
                uint32_t : 8;
                uint32_t rd : 4;
                uint32_t : 24;
                uint32_t cond : 4;
            };
        } id = {ic};

        printf("clz%s %s, %s", cc_[id.cond], rg_[id.rd], rg_[id.rm]);
    /* }}} */
    } else if ((ic & 0x0f900090) == 0x01000080 && (ic & 0xf0000000) != 0xf0000000) { /* {{{ enhanced dsp multiplies */
        union {
            uint32_t vl;

            struct {
                uint32_t rm : 4;
                uint32_t : 1;
                uint32_t x : 1;
                uint32_t y : 1;
                uint32_t : 1;
                uint32_t rs : 4;
                uint32_t rn : 4;
                uint32_t rd : 4;
                uint32_t : 1;
                uint32_t op : 2;
                uint32_t : 5;
                uint32_t cond : 4;
            };
        } id = {ic};

        static const char *oc_[] =
            {"mla", NULL, "mlal", "mul"};

        printf("%s", id.op != 0x1 ? oc_[id.op] : id.x == 0 ? "mlaw" : "mulw");
        if (id.op != 0x1)
            printf("%c", id.x == 0 ? 'b' : 't');
        printf("%c%s ", id.y == 0 ? 'b' : 't', cc_[id.cond]);
        if (id.op == 0x2)
            printf("%s, ", rg_[id.rn]);
        printf("%s, %s, %s", rg_[id.rd], rg_[id.rm], rg_[id.rs]);
        if (id.op == 0x0 || id.op == 0x1)
            printf(", %s", rg_[id.rn]);
    /* }}} */
    } else if ((ic & 0x0e000090) == 0x00000010 && (ic & 0xf0000000) != 0xf0000000 && (ic & 0x01900000) != 0x01000000) { /* {{{ data processing register shift */
        union {
            uint32_t vl;

            struct {
                uint32_t rm : 4;
                uint32_t : 1;
                uint32_t shift : 2;
                uint32_t : 1;
                uint32_t rs : 4;
                uint32_t rd : 4;
                uint32_t rn : 4;
                uint32_t s : 1;
                uint32_t opcode : 4;
                uint32_t : 3;
                uint32_t cond : 4;
            };
        } id = {ic};

        static const char *oc_[] =
            {"and", "eor", "sub", "rsb", "add", "adc", "sbc", "rsc",
             "tst", "teq", "cmp", "cmn", "orr", "mov", "bic", "mvn"};

        printf(oc_[id.opcode]);
        printf("%s", cc_[id.cond]);
        if (id.s != 0)
            printf("s");
        if ((id.opcode & 0xc) != 0x8)
            printf(" %s,", rg_[id.rd]);
        if ((id.opcode & 0xd) != 0xd)
            printf(" %s,", rg_[id.rn]);
        printf(" %s, %s %s", rg_[id.rm], sh_[id.shift], rg_[id.rs]);
    /* }}} */
    } else if ((ic & 0x0fc000f0) == 0x00000090 && (ic & 0xf0000000) != 0xf0000000) { /* {{{ multiply (accumulate) */
        union {
            uint32_t vl;

            struct {
                uint32_t rm : 4;
                uint32_t : 4;
                uint32_t rs : 4;
                uint32_t rn : 4;
                uint32_t rd : 4;
                uint32_t s : 1;
                uint32_t a : 1;
                uint32_t : 6;
                uint32_t cond : 4;
            };
        } id = {ic};

        printf("%s%s", id.a == 0 ? "mul" : "mla", cc_[id.cond]);
        if (id.s != 0)
            printf("s");
        printf(" %s, %s, %s, %s", rg_[id.rd], rg_[id.rm], rg_[id.rs], rg_[id.rn]);
    /* }}} */
    } else if ((ic & 0x0f8000f0) == 0x00800090 && (ic & 0xf0000000) != 0xf0000000) { /* {{{ multiply (accumulate) long */
        union {
            uint32_t vl;

            struct {
                uint32_t rm : 4;
                uint32_t : 4;
                uint32_t rs : 4;
                uint32_t rdlo : 4;
                uint32_t rdhi : 4;
                uint32_t s : 1;
                uint32_t a : 1;
                uint32_t u : 1;
                uint32_t : 5;
                uint32_t cond : 4;
            };
        } id = {ic};

        printf("%c%sl%s", id.u == 0 ? 'u' : 's', id.a == 0 ? "mul" : "mla", cc_[id.cond]);
        if (id.s != 0)
            printf("s");
        printf(" %s, %s, %s, %s", rg_[id.rdlo], rg_[id.rdhi], rg_[id.rm], rg_[id.rs]);
    /* }}} */
    } else if ((ic & 0x0fb000f0) == 0x01000090 && (ic & 0xf0000000) != 0xf0000000) { /* {{{ swap/swap byte */
        union {
            uint32_t vl;

            struct {
                uint32_t rm : 4;
                uint32_t : 8;
                uint32_t rd : 4;
                uint32_t rn : 4;
                uint32_t : 2;
                uint32_t b : 1;
                uint32_t : 5;
                uint32_t cond : 4;
            };
        } id = {ic};

        printf("swp");
        if (id.b)
            printf("b");
        printf("%s %s, %s, [%s]", cc_[id.cond], rg_[id.rd], rg_[id.rm], rg_[id.rn]);
    /* }}} */
/*...*/
    } else if ((ic & 0x0e000000) == 0x02000000 && (ic & 0xf0000000) != 0xf0000000 && (ic & 0x01900000) != 0x01000000) { /* {{{ data processing immediate */
        union {
            uint32_t vl;

            struct {
                uint32_t immediate : 8;
                uint32_t rotate : 4;
                uint32_t rd : 4;
                uint32_t rn : 4;
                uint32_t s : 1;
                uint32_t opcode : 4;
                uint32_t : 3;
                uint32_t cond : 4;
            };
        } id = {ic};

        static const char *oc_[] =
            {"and", "eor", "sub", "rsb", "add", "adc", "sbc", "rsc",
             "tst", "teq", "cmp", "cmn", "orr", "mov", "bic", "mvn"};

        printf(oc_[id.opcode]);
        printf("%s", cc_[id.cond]);
        if (id.s != 0)
            printf("s");
        printf(" %s", rg_[id.rd]);
        if ((id.opcode & 0xd) != 0xd)
            printf(", %s", rg_[id.rn]);
        if ((id.opcode & 0xd) == 0xd || id.immediate != 0)
            printf(", #0x%x", id.immediate << id.rotate);
    /* }}} */
    } else if ((ic & 0x0fb00000) == 0x03000000 && (ic & 0xf0000000) != 0xf0000000) { /* {{{ undefined instruction */
        printf("<undefined>");
    /* }}} */
    } else if ((ic & 0x0fb00000) == 0x03200000 && (ic & 0xf0000000) != 0xf0000000) { /* {{{ move immediate to status register */
        union {
            uint32_t vl;

            struct {
                uint32_t immediate : 4;
                uint32_t rotate : 8;
                uint32_t : 4;
                uint32_t mask : 4;
                uint32_t : 2;
                uint32_t r : 1;
                uint32_t : 5;
                uint32_t cond : 4;
            };
        } id = {ic};

        printf("msr%s %cpsr_", cc_[id.cond], id.r == 0 ? 'c' : 's');
        if ((id.mask & 0x1) != 0)
            printf("c");
        if ((id.mask & 0x2) != 0)
            printf("x");
        if ((id.mask & 0x4) != 0)
            printf("s");
        if ((id.mask & 0x8) != 0)
            printf("f");
        printf(" #0x%x", id.immediate << id.rotate);
    /* }}} */
    } else if ((ic & 0x0e000000) == 0x04000000 && (ic & 0xf0000000) != 0xf0000000) { /* {{{ load/store immediate offset */
        union {
            uint32_t vl;

            struct {
                uint32_t immediate : 12;
                uint32_t rd : 4;
                uint32_t rn : 4;
                uint32_t l : 1;
                uint32_t w : 1;
                uint32_t b : 1;
                uint32_t u : 1;
                uint32_t p : 1;
                uint32_t : 3;
                uint32_t cond : 4;
            };
        } id = {ic};

        printf("%s%s", id.l == 0 ? "str" : "ldr", cc_[id.cond]);
        if (id.b != 0)
            printf("b");
        if (id.p == 0 && id.w != 0)
            printf("t");
        printf(" %s, [", rg_[id.rd]);
        if (id.p != 0 && id.rn == 15) {
            uint32_t tg = id.u == 0 ? pc - id.immediate : pc + id.immediate;
            sy = Lookup(cx, tg, 0);
            printf("0x%.8x", tg);
        } else {
            printf("%s", rg_[id.rn]);
            if (id.p == 0)
                printf("]");
            if (id.immediate != 0)
                printf(", #%c0x%x", id.u == 0 ? '-' : '+', id.immediate);
        }
        if (id.p != 0) {
            printf("]");
            if (id.w != 0)
                printf("!");
        }
    /* }}} */
    } else if ((ic & 0x0e000010) == 0x06000000 && (ic & 0xf0000000) != 0xf0000000) { /* {{{ load/store register offset */
        union {
            uint32_t vl;

            struct {
                uint32_t rm : 4;
                uint32_t : 1;
                uint32_t shift : 2;
                uint32_t shiftamount : 5;
                uint32_t rd : 4;
                uint32_t rn : 4;
                uint32_t l : 1;
                uint32_t w : 1;
                uint32_t b : 1;
                uint32_t u : 1;
                uint32_t p : 1;
                uint32_t : 3;
                uint32_t cond : 4;
            };
        } id = {ic};

        printf("%s%s", id.l == 0 ? "str" : "ldr", cc_[id.cond]);
        if (id.b != 0)
            printf("b");
        if (id.p == 0 && id.w != 0)
            printf("t");
        printf(" %s, [", rg_[id.rd]);
        printf("%s", rg_[id.rn]);
        if (id.p == 0)
            printf("]");
        printf(", %c%s", id.u == 0 ? '-' : '+', rg_[id.rm]);
        if (id.shift == 0x3 && id.shiftamount == 0)
            printf(", rrx");
        else if (id.shift != 0x0 || id.shiftamount != 0)
            printf(", %s %u", sh_[id.shift], id.shift != 0x0 && id.shiftamount == 0 ? 32 : id.shiftamount);
        if (id.p != 0) {
            printf("]");
            if (id.w != 0)
                printf("!");
        }
    /* }}} */
    } else if ((ic & 0x0e000010) == 0x06000010 && (ic & 0xf0000000) != 0xf0000000) { /* {{{ undefined instruction */
        printf("<undefined>");
    /* }}} */
    } else if ((ic & 0xf8b00000) == 0xf0000000                                   ) { /* {{{ undefined instruction */
        printf("<undefined>");
    /* }}} */
    } else if ((ic & 0x0e000000) == 0x08000000 && (ic & 0xf0000000) != 0xf0000000) { /* {{{ load/store multiple */
        union {
            uint32_t vl;

            struct {
                uint32_t registerlist : 16;
                uint32_t rn : 4;
                uint32_t l : 1;
                uint32_t w : 1;
                uint32_t s : 1;
                uint32_t u : 1;
                uint32_t p : 1;
                uint32_t : 3;
                uint32_t cond : 4;
            };
        } id = {ic};

        printf("%sm%sXXX %s", id.l == 0 ? "st" : "ld", cc_[id.cond], rg_[id.rn]);
        if (id.w != 0)
            printf("!");
        printf(", {");
        bool cm = false;
        unsigned rg;
        for (rg = 0; rg != 16; ++rg)
            if ((id.registerlist & (1 << rg)) != 0) {
                if (cm)
                    printf(", ");
                else
                    cm = true;
                printf("%s", rg_[rg]);
            }
        printf("}");
    /* }}} */
    } else if ((ic & 0xfe000000) == 0xf8000000                                   ) { /* {{{ undefined instruction */
        printf("<undefined>");
    /* }}} */
    } else if ((ic & 0x0e000000) == 0x0a000000 && (ic & 0xf0000000) != 0xf0000000) { /* {{{ branch and branch with link */
        union {
            uint32_t vl;

            struct {
                int32_t offset : 24;
                uint32_t l : 1;
                uint32_t : 3;
                uint32_t cond : 4;
            };
        } id = {ic};

        uint32_t tg = pc + (id.offset << 2);
        sy = Lookup(cx, tg, 0);
        printf("b");
        if (id.l != 0)
            printf("l");
        printf("%s 0x%.8x", cc_[id.cond], tg);
    /* }}} */
    } else if ((ic & 0xfe000000) == 0xfa000000                                   ) { /* {{{ branch and branch with link and change to Thumb */
        union {
            uint32_t vl;

            struct {
                int32_t offset : 24;
                uint32_t h : 1;
                uint32_t : 7;
            };
        } id = {ic};

        uint32_t tg = (pc + (id.offset << 2)) | (id.h << 1);
        sy = Lookup(cx, tg, 0);
        printf("blx 0x%.8x", tg);
    /* }}} */
/*...*/
    } else if ((ic & 0x0f000010) == 0x0e000000                                   ) { /* {{{ coprocessor data processing */
        union {
            uint32_t vl;

            struct {
                uint32_t crm : 4;
                uint32_t : 1;
                uint32_t opcode2 : 3;
                uint32_t cp_num : 4;
                uint32_t crd : 4;
                uint32_t crn : 4;
                uint32_t opcode1 : 4;
                uint32_t cond : 4;
            };
        } id = {ic};

        printf("cdp%s p%u, 0x%x, c%u, c%u, c%u, 0x%x", id.cond == 0xf ? "2" : cc_[id.cond], id.cp_num, id.opcode1, id.crd, id.crn, id.crm, id.opcode2);
    /* }}} */
    } else if ((ic & 0x0f000010) == 0x0e000010                                   ) { /* {{{ coprocessor register transfers */
        union {
            uint32_t vl;

            struct {
                uint32_t crm : 4;
                uint32_t : 1;
                uint32_t opcode2 : 3;
                uint32_t cp_num : 4;
                uint32_t rd : 4;
                uint32_t crn : 4;
                uint32_t l : 1;
                uint32_t opcode1 : 3;
                uint32_t cond : 4;
            };
        } id = {ic};

        printf("%s%s p%u, 0x%x, %s, c%u, c%u, 0x%x", id.l == 0 ? "mcr" : "mrc", id.cond == 0xf ? "2" : cc_[id.cond], id.cp_num, id.opcode1, rg_[id.rd], id.crn, id.crm, id.opcode2);
    /* }}} */
    } else if ((ic & 0x0f000000) == 0x0f000000 && (ic & 0xf0000000) != 0xf0000000) { /* {{{ software interrupt */
        union {
            uint32_t vl;

            struct {
                uint32_t comment : 24;
                uint32_t : 4;
                uint32_t cond : 4;
            };
        } id = {ic};

        printf("swi%s 0x%.8x", cc_[id.cond], id.comment);
    /* }}} */
    } else if ((ic & 0xff000000) == 0xff000000                                   ) { /* {{{ undefined instruction */
        printf("<undefined>");
    /* }}} */
    } else {
        printf("<unknown>");
    }

    if (sy != NULL)
        printf(" ; %s", sy);

    return 4;
}

char *Arm11_Symbol(struct Context_ *cx, size_t ix, unsigned dp) {
    return strdup("<arm11>");
}

char *Thumb_Symbol(struct Context_ *cx, size_t ix, unsigned dp) {
    return strdup("<thumb>");
}

size_t Thumb_Format(struct Context_ *cx, size_t of) {
    uint16_t ic = * (const uint16_t *) (cx->dt + of + 0);
    uint16_t nx = * (const uint16_t *) (cx->dt + of + 2);
    uint32_t pc = Adjust(cx, of) + 4;

    const char *sy = NULL;

    size_t sz;
    if ((ic & 0xf800) == 0xf000) {
        printf("%.2x %.2x %.2x %.2x   ",
            cx->dt[of + 0],
            cx->dt[of + 1],
            cx->dt[of + 2],
            cx->dt[of + 3]
        );

        sz = 4;
    } else {
        printf("%.2x %.2x         ",
            cx->dt[of + 0],
            cx->dt[of + 1]
        );

        sz = 2;
    }

    if (false) {
    } else if ((ic & 0xe000) == 0x0000 && (ic & 0x1800) != 0x1800) { /* {{{ shift by immediate */
        union {
            uint16_t vl;

            struct {
                uint16_t rd : 3;
                uint16_t rm : 3;
                uint16_t immediate : 5;
                uint16_t opcode : 2;
                uint16_t : 3;
            };
        } id = {ic};

        static const char *oc_[] =
            {"lsl", "lsr", "asr", NULL};

        if (id.opcode == 0 && id.immediate == 0)
            printf("movs %s, %s", rg_[id.rd], rg_[id.rm]);
        else
            printf("%ss %s, %s, #%u", oc_[id.opcode], rg_[id.rd], rg_[id.rm], id.immediate == 0 ? 32 : id.immediate);
    /* }}} */
    } else if ((ic & 0xfc00) == 0x1800) { /* {{{ add/subtract register */
        union {
            uint16_t vl;

            struct {
                uint16_t rd : 3;
                uint16_t rn : 3;
                uint16_t rm : 3;
                uint16_t opc : 1;
                uint16_t : 6;
            };
        } id = {ic};

        printf("%ss %s, %s, %s", id.opc == 0 ? "add" : "sub", rg_[id.rd], rg_[id.rn], rg_[id.rm]);
    /* }}} */
    } else if ((ic & 0xfc00) == 0x1c00) { /* {{{ add/subtract immediate */
        union {
            uint16_t vl;

            struct {
                uint16_t rd : 3;
                uint16_t rn : 3;
                uint16_t immediate : 3;
                uint16_t opc : 1;
                uint16_t : 6;
            };
        } id = {ic};

        printf("%ss %s, %s", id.immediate == 0 ? "mov" : id.opc == 0 ? "add" : "sub", rg_[id.rd], rg_[id.rn]);
        if (id.immediate != 0)
            printf(", #0x%x", id.immediate);
    /* }}} */
    } else if ((ic & 0xe000) == 0x2000) { /* {{{ add/subtract/compare/move immediate */
        union {
            uint16_t vl;

            struct {
                uint16_t immediate : 8;
                uint16_t rdn : 3;
                uint16_t opcode : 2;
                uint16_t : 3;
            };
        } id = {ic};

        static const char *oc_[] =
            {"movs", "cmp", "adds", "subs"};

        printf("%s %s, #0x%x", oc_[id.opcode], rg_[id.rdn], id.immediate);
    /* }}} */
    } else if ((ic & 0xfc00) == 0x4000) { /* {{{ data-processing register */
        union {
            uint16_t vl;

            struct {
                uint16_t rdn : 3;
                uint16_t rms : 3;
                uint16_t opcode : 4;
                uint16_t : 6;
            };
        } id = {ic};

        static const char *op_[] =
            {"and", "eor", "lsl", "lsr", "asr", "adc", "sbc", "ror",
             "tst", "rsb", "cmp", "cmn", "orr", "mul", "bic", "mvn"};

        printf("%s %s, %s", op_[id.opcode], rg_[id.rdn], rg_[id.rms]);
        if (id.opcode == 0x9)
            printf(", #0");
    /* }}} */
    } else if ((ic & 0xfc00) == 0x4400 && (ic & 0x0300) != 0x0300) { /* {{{ special data processing */
        union {
            uint16_t vl;

            struct {
                uint16_t rdn : 3;
                uint16_t rm : 3;
                uint16_t h2 : 1;
                uint16_t h1 : 1;
                uint16_t opcode : 2;
                uint16_t : 6;
            };
        } id = {ic};

        static const char *oc_[] =
            {"add", "cmp", "mov", NULL};

        printf("%s %s, %s", oc_[id.opcode], rg_[id.h1 << 3 | id.rdn], rg_[id.h2 << 3 | id.rm]);
    /* }}} */
    } else if ((ic & 0xff00) == 0x4700) { /* {{{ branch/exchange instruction set */
        union {
            uint16_t vl;

            struct {
                uint16_t : 3;
                uint16_t rm : 3;
                uint16_t h2 : 1;
                uint16_t l : 1;
                uint16_t : 8;
            };
        } id = {ic};

        printf("b");
        if (id.l != 0)
            printf("l");
        printf("x %s", rg_[id.h2 << 3 | id.rm]);
    /* }}} */
    } else if ((ic & 0xf800) == 0x4800) { /* {{{ load from literal pool */
        union {
            uint16_t vl;

            struct {
                uint16_t imm8 : 8;
                uint16_t rt : 3;
                uint16_t : 5;
            };
        } id = {ic};

        uint32_t tg = (pc & ~0x3) + (id.imm8 << 2);
        sy = Lookup(cx, tg, 0);
        printf("ldr %s, [0x%.8x]", rg_[id.rt], tg);
    /* }}} */
    } else if ((ic & 0xf000) == 0x5000) { /* {{{ load/store register offset */
        union {
            uint16_t vl;

            struct {
                uint16_t rd : 3;
                uint16_t rn : 3;
                uint16_t rm : 3;
                uint16_t opcode : 3;
                uint16_t : 4;
            };
        } id = {ic};

        static const char *op_[] =
            {"str", "strh", "strb", "ldrsb",
             "ldr", "ldrh", "ldrb", "ldrsh"};

        printf("%s %s, [%s, %s]", op_[id.opcode], rg_[id.rd], rg_[id.rn], rg_[id.rm]);
    /* }}} */
    } else if ((ic & 0xe000) == 0x6000) { /* {{{ load/store word/byte immediate offset */
        union {
            uint16_t vl;

            struct {
                uint16_t rd : 3;
                uint16_t rn : 3;
                uint16_t offset : 5;
                uint16_t l : 1;
                uint16_t b : 1;
                uint16_t : 3;
            };
        } id = {ic};

        printf("%s", id.l == 0 ? "str" : "ldr");
        if (id.b != 0)
            printf("b");
        printf(" %s, [%s", rg_[id.rd], rg_[id.rn]);
        if (id.offset != 0)
            printf(", #0x%x", id.offset << 2);
        printf("]");
    /* }}} */
    } else if ((ic & 0xf000) == 0x8000) { /* {{{ load/store halfword immediate offset */
        union {
            uint16_t vl;

            struct {
                uint16_t rd : 3;
                uint16_t rn : 3;
                uint16_t offset : 5;
                uint16_t l : 1;
                uint16_t : 4;
            };
        } id = {ic};

        printf("%srh %s, [%s", id.l == 0 ? "st" : "ld", rg_[id.rd], rg_[id.rn]);
        if (id.offset != 0)
            printf(", #0x%x * 2", id.offset);
        printf("]");
    /* }}} */
    } else if ((ic & 0xf000) == 0x9000) { /* {{{ load/store to/from stack */
        union {
            uint16_t vl;

            struct {
                uint16_t sprelativeoffset : 8;
                uint16_t rd : 3;
                uint16_t l : 1;
                uint16_t : 4;
            };
        } id = {ic};

        printf("%sr %s, [sp, #0x%x * 4]", id.l == 0 ? "st" : "ld", rg_[id.rd], id.sprelativeoffset);
    /* }}} */
    } else if ((ic & 0xf000) == 0xa000) { /* {{{ add to sp or pc */
        union {
            uint16_t vl;

            struct {
                uint16_t immediate : 8;
                uint16_t rd : 3;
                uint16_t sp : 1;
                uint16_t : 4;
            };
        } id = {ic};

        printf("add %s, %s, #0x%x * 4", rg_[id.rd], id.sp == 0 ? "pc" : "sp", id.immediate);
    /* }}} */
    } else if ((ic & 0xff00) == 0xb000) { /* {{{ adjust stack pointer */
        union {
            uint16_t vl;

            struct {
                uint16_t immediate : 7;
                uint16_t opc : 1;
                uint16_t : 8;
            };
        } id = {ic};

        printf("%s sp, #0x%x * 4", id.opc == 0 ? "add" : "sub", id.immediate);
    /* }}} */
    } else if ((ic & 0xff00) == 0xb200) { /* {{{ extend register byte/halfword */
        union {
            uint16_t vl;

            struct {
                uint16_t rd : 3;
                uint16_t rm : 3;
                uint16_t b : 1;
                uint16_t u : 1;
                uint16_t : 8;
            };
        } id = {ic};

        printf("%cxt%c %s, %s", id.u == 0 ? 's' : 'u', id.b == 0 ? 'h' : 'b', rg_[id.rd], rg_[id.rm]);
    /* }}} */
    } else if ((ic & 0xf600) == 0xb400) { /* {{{ push/pop register list */
        union {
            uint16_t vl;

            struct {
                uint16_t registerlist : 8;
                uint16_t r : 1;
                uint16_t : 2;
                uint16_t l : 1;
                uint16_t : 4;
            };
        } id = {ic};

        printf("%s {", id.l == 0 ? "push" : "pop");
        bool cm = false;
        unsigned rg;
        for (rg = 0; rg != 8; ++rg)
            if ((id.registerlist & (1 << rg)) != 0) {
                if (cm)
                    printf(", ");
                else
                    cm = true;
                printf("%s", rg_[rg]);
            }
        if (id.r != 0) {
            if (cm)
                printf(", ");
            else
                cm = true;
            printf("%s", rg_[id.l == 0 ? 14 : 15]);
        }
        printf("}");
    /* }}} */
    } else if ((ic & 0xff00) == 0xba00 && (ic & 0x00c0) != 0x0080) { /* {{{ byte-reverse word/packed halfword/signed halfword */
        union {
            uint16_t vl;

            struct {
                uint16_t rd: 3;
                uint16_t rm : 3;
                uint16_t opcode : 2;
                uint16_t : 10;
            };
        } id = {ic};

        static const char *oc_[] =
            {"rev", "rev16", NULL, "revsh"};

        printf("%s %s, %s", oc_[id.opcode], rg_[id.rd], rg_[id.rm]);
    /* }}} */
    } else if ((ic & 0xff00) == 0xbe00) { /* {{{ software breakpoint */
        union {
            uint16_t vl;

            struct {
                uint16_t immediate : 8;
                uint16_t : 8;
            };
        } id = {ic};

        printf("bkpt 0x%.2x", id.immediate);
    /* }}} */
    } else if ((ic & 0xf000) == 0xc000) { /* {{{ load/store multiple */
        union {
            uint16_t vl;

            struct {
                uint16_t registerlist : 8;
                uint16_t rn : 3;
                uint16_t l : 1;
                uint16_t : 4;
            };
        } id = {ic};

        printf("%smia %s!, {", id.l == 0 ? "st" : "ld", rg_[id.rn]);
        bool cm = false;
        unsigned rg;
        for (rg = 0; rg != 8; ++rg)
            if ((id.registerlist & (1 << rg)) != 0) {
                if (cm)
                    printf(", ");
                else
                    cm = true;
                printf("%s", rg_[rg]);
            }
        printf("}");
    /* }}} */
    } else if ((ic & 0xf000) == 0xd000 && (ic & 0x0f00) != 0x0e00) { /* {{{ conditional branch */
        union {
            uint16_t vl;

            struct {
                int16_t offset : 8;
                uint16_t cond : 4;
                uint16_t : 4;
            };
        } id = {ic};

        uint32_t tg = pc + (id.offset << 1);
        sy = Lookup(cx, tg, 0);
        printf("b%s 0x%.8x", cc_[id.cond], tg);
    /* }}} */
    } else if ((ic & 0xff00) == 0xde00) { /* {{{ permanently undefined */
        printf("<undefined>");
    /* }}} */
    } else if ((ic & 0xff00) == 0xdf00) { /* {{{ software interrupt */
        union {
            uint16_t vl;

            struct {
                uint16_t immediate : 8;
                uint16_t : 8;
            };
        } id = {ic};

        printf("swi 0x%.2x", id.immediate);
    /* }}} */
    } else if ((ic & 0xf800) == 0xe000) { /* {{{ unconditional branch */
        union {
            uint16_t vl;

            struct {
                int16_t offset : 11;
                uint16_t : 5;
            };
        } id = {ic};

        uint32_t tg = pc + (id.offset << 1);
        sy = Lookup(cx, tg, 0);
        printf("b 0x%.8x", tg);
    /* }}} */
    } else if ((ic & 0xfe40) == 0xe800) { /* {{{ load/store multiple */
    /* }}} */
    } else if ((ic & 0xf800) == 0xf000 && (nx & 0xf800) == 0xe800) { /* {{{ bl/blx prefix */
        union {
            uint16_t vl;

            struct {
                uint16_t offset : 10;
                uint16_t s : 1;
                uint16_t : 5;
            };
        } id = {ic};

        union {
            uint16_t vl;

            struct {
                uint16_t offset : 11;
                uint16_t j2 : 1;
                uint16_t x : 1;
                uint16_t j1 : 1;
                uint16_t : 2;
            };
        } nd = {nx};

        int32_t os = 0;
        os |= id.s << 24;
        os |= (~(id.s ^ nd.j1) & 0x1) << 23;
        os |= (~(id.s ^ nd.j2) & 0x1) << 22;
        os |= id.offset << 12;
        os |= nd.offset << 1;
        os |= nd.x;
        os <<= 7;
        os >>= 7;

        printf("bl");
        if (nd.x == 0)
            printf("x");
        uint32_t tg = pc + os;
        if (nd.x == 0 && (tg & 0x1) == 0)
            tg &= ~0x2;
        sy = Lookup(cx, tg, 0);
        printf(" 0x%.8x", tg);
    /* }}} */
    } else {
        printf("<unknown>");
    }

    if (sy != NULL)
        printf(" ; %s", sy);

    return sz;
}

bool Arm11_Prefer(struct Context_ *cx, size_t ix) {
    uint32_t vl = * (const uint32_t *) (cx->dt + ix);

    return ix % 4 == 0 /*&& (
        ((vl & 0x0ff000f0) == 0x01200010 && (vl & 0xf0000000) != 0xf0000000) ||
        ((vl & 0x0ff000f0) == 0x01200030 && (vl & 0xf0000000) != 0xf0000000) ||
        ((vl & 0x0fb000f0) == 0x01000090 && (vl & 0xf0000000) != 0xf0000000) ||
        ((vl & 0x0e000000) == 0x0a000000 && (vl & 0xf0000000) != 0xf0000000) ||
         (vl & 0xfe000000) == 0xfa000000
    )*/;
}

bool Thumb_Prefer(struct Context_ *cx, size_t ix) {
    uint16_t vl = * (const uint16_t *) (cx->dt + ix);

    return ix % 2 == 0 /*&& (
         (vl & 0xff00) == 0x4700 ||
         (vl & 0xf800) == 0x4800 ||
        ((vl & 0xf000) == 0xd000 && (vl & 0x0f00) != 0x0e00) ||
         (vl & 0xf800) == 0xe000 ||
         (vl & 0xf800) == 0xf000
    )*/;
}

size_t Magic_Format(struct Context_ *cx, size_t of) {
    uint32_t vl = * (const uint32_t *) (cx->dt + of);

    printf("%.2x %.2x %.2x %.2x   ",
        cx->dt[of + 0],
        cx->dt[of + 1],
        cx->dt[of + 2],
        cx->dt[of + 3]
    );

    printf("== '%c%c%c%c'/'%c%c%c%c'/%.8x",
        Characterize(vl >> 24 & 0xff),
        Characterize(vl >> 16 & 0xff),
        Characterize(vl >>  8 & 0xff),
        Characterize(vl >>  0 & 0xff),
        Characterize(vl >>  0 & 0xff),
        Characterize(vl >>  8 & 0xff),
        Characterize(vl >> 16 & 0xff),
        Characterize(vl >> 24 & 0xff),
        vl
    );

    return 4;
}

size_t Trash_Format(struct Context_ *cx, size_t ix) {
    if (ix % 2 != 0 || cx->sz - ix == 1 || cx->fm[ix + 1] != 0)
        return UInt1_Format(cx, ix);
    else if (ix % 4 != 0 || cx->sz - ix < 4 || cx->fm[ix + 2] != 0 || cx->fm[ix + 3] != 0)
        return UInt2_Format(cx, ix);
    else
        return UInt4_Format(cx, ix);
}

void Trash_Follow(struct Context_ *cx, size_t ix, size_t sz) {
    struct Formatter_ *fm;
    for (; sz != 0; ++ix, --sz)
    for (fm = fm_; fm != NULL; fm = fm->nx)
        if (fm->pf != NULL && (*fm->pf)(cx, ix)) {
            printf("                     <%s> ;   ", fm->nm);
            fm->fm(cx, ix);
            printf("\n");
        }
}

size_t Ehffs_Format(struct Context_ *cx, size_t ix) {
    size_t sz = 0;
    while (cx->fm[ix] == 11) {
        _assert(cx->dt[ix] == 0xff);
        ++ix;
        ++sz;
    }

    printf("ff ..");
    return sz;
}

size_t Zeros_Format(struct Context_ *cx, size_t ix) {
    size_t sz = 0;
    while (cx->fm[ix] == 5) {
        _assert(cx->dt[ix] == 0x00);
        ++ix;
        ++sz;
    }

    printf("00 ..");
    return sz;
}

char *Trash_Symbol(struct Context_ *cx, size_t ix, unsigned dp) {
    if (ix % 2 != 0 || cx->sz - ix == 1 || cx->fm[ix + 1] != 0)
        return UInt1_Symbol(cx, ix, dp);
    else if (ix % 4 != 0 || cx->sz - ix < 4 || cx->fm[ix + 2] != 0 || cx->fm[ix + 3] != 0)
        return UInt2_Symbol(cx, ix, dp);

    uint32_t vl = * (const uint32_t *) (cx->dt + ix);
    if (Locate(cx, vl, NULL))
        return Point_Symbol(cx, ix, dp);

    return UInt4_Symbol(cx, ix, dp);
}

struct Formatter_ Trash = {
    .id = 0x0,
    .nm = "trash",
    .fm = &Trash_Format,
    .sy = &Trash_Symbol,
    .fl = &Trash_Follow
};

struct Formatter_ UInt2 = {
    .id = 0x1,
    .nm = "uint2",
    .fm = &UInt2_Format,
    .sy = &UInt2_Symbol
};

struct Formatter_ UInt4 = {
    .id = 0x2,
    .nm = "uint4",
    .fm = &UInt4_Format,
    .sy = &UInt4_Symbol
};

struct Formatter_ Arm11 = {
    .id = 0x3,
    .nm = "arm11",
    .fm = &Arm11_Format,
    .sy = &Arm11_Symbol,
    .pf = &Arm11_Prefer
};

struct Formatter_ Thumb = {
    .id = 0x4,
    .nm = "thumb",
    .fm = &Thumb_Format,
    .sy = &Thumb_Symbol,
    .pf = &Thumb_Prefer
};

struct Formatter_ Zeros = {
    .id = 0x5,
    .nm = "zeros",
    .fm = &Zeros_Format,
    .sy = &Zeros_Symbol
};

struct Formatter_ Point = {
    .id = 0x6,
    .nm = "point",
    .fm = &Point_Format,
    .sy = &Point_Symbol
};

struct Formatter_ Table = {
    .id = 0x7,
    .nm = "table",
    .fm = &Table_Format,
    .sy = &Table_Symbol
};

struct Formatter_ Magic = {
    .id = 0x8,
    .nm = "magic",
    .fm = &Magic_Format,
    .sy = &Magic_Symbol
};

struct Formatter_ Chars = {
    .id = 0x9,
    .nm = "chars",
    .fm = &Chars_Format,
    .sy = &Chars_Symbol
};

struct Formatter_ UInt1 = {
    .id = 0xa,
    .nm = "uint1",
    .fm = &UInt1_Format,
    .sy = &UInt1_Symbol
};

struct Formatter_ Ehffs = {
    .id = 0xb,
    .nm = "ehffs",
    .fm = &Ehffs_Format,
    .sy = &Ehffs_Symbol
};

struct Formatter_ *Format(uint8_t id) {
    struct Formatter_ *fm;
    for (fm = fm_; fm != NULL; fm = fm->nx)
        if (fm->id == id)
            return fm;
    return NULL;
}

char *Lookup(struct Context_ *cx, pointer_t of, unsigned dp) {
    size_t ix;
    char *sy = Find(cx, of);
    if (sy != NULL)
        return sy;
    if (!Locate(cx, of, &ix))
        return NULL;
    struct Formatter_ *fm = Format(cx->fm[ix]);
    return fm->sy == NULL ? NULL : (*fm->sy)(cx, ix, dp);
}

void Register(struct Formatter_ *fm) {
    fm->nx = fm_;
    fm_ = fm;
}

int main(int ac, const char *av[]) {
    _assert(ac == 5);

    Register(&Trash);
    Register(&UInt2);
    Register(&UInt4);
    Register(&Arm11);
    Register(&Thumb);
    Register(&Zeros);
    Register(&Point);
    Register(&Table);
    Register(&Magic);
    Register(&Chars);
    Register(&UInt1);
    Register(&Ehffs);

    const char *fl = av[3];
    const char *ss = av[4];

    struct Context_ *cx = malloc(sizeof(struct Context_));
    cx->bs = strtoul(av[1], NULL, 0);
    cx->dt = map(av[2], 0, _not(size_t), &cx->sz, true);

    _assert(cx->dt != NULL);

    cx->fm = map(fl, 0, _not(size_t), NULL, true);
    _assert(cx->fm != NULL);

    FILE *fn = fopen(ss, "r");
    _assert(fn != NULL);

    cx->sy = NULL;

    uint32_t of;
    char nm[1024];
    while (fscanf(fn, "0x%x %[^\n]\n", &of, nm) != EOF) {
        struct Symbol_ *sy = malloc(sizeof(struct Symbol_));
        sy->nx = cx->sy;
        sy->of = of;
        sy->nm = strdup(nm);
        cx->sy = sy;
    }

    fclose(fn);

    size_t ix;
    for (ix = 0; ix < cx->sz; ) {
        pointer_t of = Adjust(cx, ix);

        char *sy = Find(cx, of);
        if (sy != NULL) {
            printf("================== %s ==================\n", sy);
            free(sy);
        }

        struct Formatter_ *fm = Format(cx->fm[ix]);

        printf("(0x%.6zx:%s)   0x%.8x:   ", ix, fm->nm, of);
        size_t sz = fm->fm(cx, ix);
        printf("\n");
        if (fm->fl != NULL)
            (*fm->fl)(cx, ix, sz);
        ix += sz;
    }

    _assert(ix == cx->sz);

    return 0;
}
