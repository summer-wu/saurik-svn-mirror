#include "minimal/stdlib.h"
#include "minimal/mapping.h"

#include <memory.h>
#include <arpa/inet.h>

#define bint32(x) ntohl(x)
#define lint32(x) x

#define bint16(x) ntohs(x)
#define lint16(x) x

struct Header8900_ {
    char magic_[4];
    char version_[3];
    uint8_t format_;
    uint32_t null_;
    uint32_t size_;
    uint32_t signature_;
    uint32_t footer_;
    uint32_t length_;
    uint8_t data0_[32];
    uint32_t flags_;
    uint8_t data1_[16];
    uint8_t padding_[0x30];
    uint8_t space_[0x780];
};

struct HeaderImg2_ {
    char magic_[4];
    char name_[4];
};

void printh(uint8_t *data, size_t size) {
    size_t i;
    for (i = 0; i != size; ++i)
        printf("%.2x", data[i]);
}

void Analyze(const uint8_t *data, size_t size) {
    struct Header8900_ *header8900 = (struct Header8900_ *) data;
    struct HeaderImg2_ *headerImg2 = (struct HeaderImg2_ *) data;

    if (memcmp(header8900->magic_, "8900", 4) == 0) {
        _assert(header8900->null_ == 0x00000000);

        printf("<8900\n");

        printf("    version=\"%c%c%c\"\n",
            header8900->version_[0],
            header8900->version_[1],
            header8900->version_[2]
        );

        printf("    format=\"%u\"\n", header8900->format_);
        printf("    size=\"%u\"\n", header8900->size_);

        printf("    data0=\"");
        printh(header8900->data0_, sizeof(header8900->data0_));
        printf("\"\n");

        printf("    flags=\"%.8x\"\n", header8900->flags_);

        printf("    data1=\"");
        printh(header8900->data1_, sizeof(header8900->data1_));
        printf("\"\n");

        printf(">\n");

        Analyze(
            data + sizeof(struct Header8900_),
            size - sizeof(struct Header8900_)
        );

        printf("</8900>\n");
    } else if (memcmp(headerImg2->magic_, "2gmI", 4) == 0) {
        printf("<Img2\n");

        printf("    name=\"%c%c%c%c\"\n",
            headerImg2->name_[3],
            headerImg2->name_[2],
            headerImg2->name_[1],
            headerImg2->name_[0]
        );

        printf("/>\n");
    } else {
        printf("<!-- unknown -->\n");
    }
}

int main(int argc, const char *argv[]) {
    _assert(argc == 2);

    const char *path = argv[1];
    size_t size;
    void *base = map(path, 0, _not(size_t), &size, true);

    Analyze(base, size);

    return 0;
}
