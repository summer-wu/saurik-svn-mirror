#include "platform.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

void fatal(const char *reason) {
    perror(reason);
    exit(1);
}

struct order {
    uint8_t value_;
    size_t count_;
};

struct table {
    size_t size_;

    union {
        struct {
            const char *name_;
            uintptr_t node_;
        } values_;

        struct table *children_[2];
    } data_;

    size_t counts0_[256];
    size_t counts1_[256][256];

    struct order ordered_[256];

    double distance_;
    double *distances_;
};

double log_2;

unsigned I = 0;

_finline double distance_(size_t lvalue, size_t lsize, size_t rvalue, size_t rsize) {
    double p = lvalue / (double) lsize;
    double q = rvalue / (double) rsize;

    if (p == q)
        return 0.0;

    /*if (p == 0.0 || q == 0.0)
        return 0.0;*/

    if (p == 0.0)
        p = __DBL_EPSILON__;
    if (q == 0.0)
        q = __DBL_EPSILON__;

    //printf("p=%f q=%f d=%f\n", p, q, disbelief);

    return p * log(p / q) + q * log(q / p);
}

double distance(struct table *lhs, struct table *rhs) {
    //printf("distance() %i\n", ++I);
    double distance = 1.0;

    /*{
        double disbelief = 0.0;
        for (unsigned i = 0; i != 256; ++i)
            disbelief += distance_(lhs->ordered_[i].count_, lhs->size_, rhs->ordered_[i].count_, rhs->size_) * (
                lhs->ordered_[i].value_ == rhs->ordered_[i].value_ ? 1.0 : 1.0
            );

        distance *= disbelief;
    }*/

    {
        double disbelief = 0.0;

        for (unsigned i = 0; i != 256; ++i)
            disbelief += distance_(lhs->counts0_[i], lhs->size_, rhs->counts0_[i], rhs->size_);

        distance *= disbelief;
    }

    /*{
        double disbelief = 0.0;

        for (unsigned i = 0; i != 256; ++i)
            for (unsigned j = 0; j != 256; ++j)
                disbelief += distance_(lhs->counts1_[lhs->ordered_[i].value_][lhs->ordered_[j].value_], lhs->size_, rhs->counts1_[rhs->ordered_[i].value_][rhs->ordered_[j].value_], rhs->size_);

        distance *= disbelief;
    }*/

    {
        double disbelief = 0.0;

        for (unsigned i = 0; i != 256; ++i)
            for (unsigned j = 0; j != 256; ++j)
                disbelief += distance_(lhs->counts1_[i][j], lhs->size_, rhs->counts1_[i][j], rhs->size_);

        distance *= disbelief;
    }

    /*{
        double correlation = 0.0;
        for (unsigned i = 0; i != 256; ++i)
            for (unsigned j = 0; j != 256; ++j)
                if (lhs->ordered_[i].value_ == rhs->ordered_[j].value_) {
                    correlation += (lhs->ordered_[i].count_ / (double) lhs->size_ + rhs->ordered_[j].count_ / (double) rhs->size_) / pow(2.0, fabs((signed) j - (signed) i));
                    break;
                }

        printf("%7.4f /= %7.4f\n", distance, correlation);
        distance /= correlation;
    }*/

    _assert(isfinite(distance));
    return distance / log_2;
}

void output(struct table *table, unsigned level, bool levels[], bool second) {
        for (unsigned i = 0; i != level; ++i)
            printf(" %c", levels[i] ? '|' : ' ');
        printf(second ? " \\" : " +");

    if (table->data_.values_.node_) {
        printf("-+ [%f]\n", table->distance_);
        levels[++level] = true;
        output(table->data_.children_[0], level, levels, false);
        levels[level] = false;
        output(table->data_.children_[1], level, levels, true);
    } else {
        printf("- %s\n", table->data_.values_.name_);
    }
}

int compare(const void *lhs, const void *rhs) {
    struct order *lorder = (struct order *) lhs;
    struct order *rorder = (struct order *) rhs;
    return (int) lorder->count_ - (int) rorder->count_;
}

void order(struct table *table) {
    for (unsigned i = 0; i != 256; ++i) {
        table->ordered_[i].value_ = i;
        table->ordered_[i].count_ = table->counts0_[i];
    }

    qsort(table->ordered_, 256, sizeof(struct order), &compare);
}

int main(int argc, const char *argv[]) {
    log_2 = log(2);

    size_t otables = argc - 1, stables = otables;
    _assert(stables != 0);

    struct table **ptables = malloc(sizeof(struct table *) * stables);

    for (size_t i = 0; i != stables; ++i) {
        const char *name = argv[i + 1];
        printf("input: %s\n", name);

        int fd;
        _syscall(fd = open(name, O_RDONLY));

        struct stat stat;
        _syscall(fstat(fd, &stat));

        _assert(stat.st_size != 0);

        uint8_t *map;
        _syscall(map = mmap(NULL, stat.st_size, PROT_READ, MAP_SHARED, fd, 0));

        struct table *table = malloc(sizeof(struct table));
        memset(table, 0, sizeof(table));

        ptables[i] = table;

        table->size_ = stat.st_size;
        table->data_.values_.name_ = name;

        for (size_t c = 0; c != stat.st_size; ++c)
            ++table->counts0_[map[c]];

        for (size_t c = 1; c != stat.st_size; ++c)
            ++table->counts1_[map[c - 1]][map[c]];

        /*for (size_t c = 2; c != stat.st_size; ++c)
            ++table->counts2_[map[c - 2]][map[c - 1]][map[c]];*/

        _syscall(munmap(map, stat.st_size));
        _syscall(close(fd));

        order(table);

        for (unsigned i = 0; i != 20; ++i) {
            uint8_t value = table->ordered_[255 - i].value_;
            printf("[%c:%5.2f] ", value < 128 && value >= 32 ? value : '?', table->ordered_[255 - i].count_ * 100.0 / table->size_);
        }

        printf("\n");
    }

    printf("\n");

    for (size_t i = 0; i != stables; ++i) {
        ptables[i]->distances_ = malloc(sizeof(double) * stables);

        for (size_t j = i + 1; j != stables; ++j) {
            double cdistance = distance(ptables[i], ptables[j]);
            printf("  D(%3i,%3i) = %f <%s, %s>\n", i, j, cdistance, ptables[i]->data_.values_.name_, ptables[j]->data_.values_.name_);
            ptables[i]->distances_[j] = cdistance;
            //printf("*");
            //fflush(stdout);
        }

        //printf("\n");
    }

    while (stables != 1) {
        double mdistance = INFINITY;
        size_t pair[2] = {_not(size_t), _not(size_t)};

        for (size_t i = 0; i != stables; ++i)
            for (size_t j = i + 1; j != stables; ++j) {
                double cdistance = ptables[i]->distances_[j];
                if (mdistance > cdistance) {
                    mdistance = cdistance;
                    pair[0] = i;
                    pair[1] = j;
                }
            }

        printf("[%2d%%] merge: %3u <- %3u (%f)\n", (otables - stables) * 100 / otables, pair[0], pair[1], mdistance);

        struct table *first = ptables[pair[0]];
        struct table *second = ptables[pair[1]];

        struct table *table = malloc(sizeof(struct table));
        table->size_ = first->size_ + second->size_;
        table->data_.children_[0] = first;
        table->data_.children_[1] = second;

        table->distance_ = mdistance;

        for (unsigned i = 0; i != 256; ++i)
            table->counts0_[i] = first->counts0_[i] + second->counts0_[i];

        for (unsigned i = 0; i != 256; ++i)
            for (unsigned j = 0; j != 256; ++j)
                table->counts1_[i][j] = first->counts1_[i][j] + second->counts1_[i][j];

        ptables[pair[0]] = table;

        for (size_t i = pair[1] + 1; i != stables; ++i)
            ptables[i - 1] = ptables[i];

        for (size_t i = 0; i != stables - 1; ++i)
            if (i != pair[0])
                for (size_t j = pair[1] + 1; j != stables; ++j)
                    ptables[i]->distances_[j - 1] = ptables[i]->distances_[j];

        --stables;

        table->distances_ = malloc(sizeof(double) * stables);

        order(table);

        for (size_t i = 0; i != pair[0]; ++i)
            ptables[i]->distances_[pair[0]] = distance(ptables[i], table);

        for (size_t i = pair[0] + 1; i != stables; ++i)
            table->distances_[i] = distance(table, ptables[i]);
    }

    printf("\n");

    bool levels[otables];
    memset(levels, 0, sizeof(levels));
    output(ptables[0], 0, levels, false);

    return 0;
}
