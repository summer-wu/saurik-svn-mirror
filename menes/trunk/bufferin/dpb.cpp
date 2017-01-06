/* Bufferin - Schema-less Protocol Buffer Library
 * Copyright (C) 2009  Jay Freeman (saurik)
*/

/* Modified BSD License {{{ */
/*
 *        Redistribution and use in source and binary
 * forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer in the documentation
 *    and/or other materials provided with the
 *    distribution.
 * 3. The name of the author may not be used to endorse
 *    or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/* }}} */

#include <stdint.h>
#include <vector>
#include <deque>
#include <cstdio>
#include <iostream>
#include <iomanip>

uint64_t offset;

uint8_t byte() {
    int c(getchar());
    if (c == EOF)
        throw "early end-of-file";
    ++offset;
    return c;
}

uint64_t varint() {
    uint64_t n(0);
    unsigned s(0);
    uint8_t b;

    do {
        b = byte();
        n |= (b & ~0x80) << s;
        s += 7;
    } while ((b & 0x80) != 0);

    return n;
}

void buffer(uint64_t max, bool group, std::vector< std::deque<uint64_t> > paths) {
    uint64_t end;
    if (max == ~uint64_t())
        end = ~uint64_t();
    else
        end = offset + max;

    bool comma(false);

    for (;;) {
        if (offset > end)
            throw "buffer overrun";
        else if (offset == end)
            return;

        uint64_t key;

        try {
            key = varint();
        } catch (const char *e) {
            if (max == ~uint64_t())
                return;
            else
                throw e;
        }

        uint64_t field(key >> 3);
        uint64_t type(key & 0x7);

        if (type == 4)
            return;

        if (comma)
            std::cout << ',';
        else
            comma = true;

        std::cout << std::setbase(10);
        std::cout << field << ":";

        bool hit(false);

        std::vector< std::deque<uint64_t> > subpaths(paths);
        for (size_t i(0); i != subpaths.size(); ++i)
            if (subpaths[i][0] != field) erase:
                subpaths.erase(subpaths.begin() + i--);
            else if (subpaths[i].size() != 1)
                subpaths[i].pop_front();
            else {
                hit = true;
                goto erase;
            }

        switch (type) {
            case 0: {
                uint64_t v(varint());
                std::cout << std::setbase(10);
                std::cout << v;
            } break;

            case 1:
                throw "unimplemented type";

            case 2: {
                uint64_t l(varint());
                if (hit) {
                    std::cout << '[';
                    buffer(l, false, subpaths);
                    std::cout << ']';
                } else {
                    std::cout << '\"';
                    for (uint64_t i(0); i != l; ++i) {
                        char c(byte());
                        if (c == '\n')
                            std::cout << "\\n";
                        else if (c == '\r')
                            std::cout << "\\r";
                        else if (c == '\t')
                            std::cout << "\\t";
                        else if (c < 0x20 || c >= 0x7f) {
                            std::cout << "\\x" << std::setbase(16) << std::setw(2) << std::setfill('0');
                            std::cout << static_cast<unsigned>(c & 0xff);
                        } else {
                            if (c == '\'' || c == '"' || c == '\\' || c == '$' || c == '`')
                                std::cout << '\\';
                            std::cout << c;
                        }
                    }
                    std::cout << '\"';
                }
            } break;

            case 3:
                std::cout << '{';
                buffer(~uint64_t(), true, subpaths);
                std::cout << '}';
            break;

            case 4:
                throw "mismatched group";

            case 5: {
                uint32_t v(0);
                v |= byte() <<  0;
                v |= byte() <<  8;
                v |= byte() << 16;
                v |= byte() << 24;
                std::cout << '+';
                std::cout << std::setbase(10);
                std::cout << v;
            } break;

            default:
                throw "invalid type";
        }
    }
}

int main(int argc, char *argv[]) {
    try {
        if (argc == 0)
            throw "invalid arguments";
        std::vector< std::deque<uint64_t> > paths;

        for (int argi(1); argi != argc; ++argi) {
            char *arga(argv[argi]);
            std::deque<uint64_t> path;
            do {
                char *slash(strchr(arga, '/'));
                if (slash != NULL)
                    *slash = '\0';
                char *end;
                path.push_back(strtoull(arga, &end, 0));
                if (end != (slash == NULL ? arga + strlen(arga) : slash))
                    throw "invalid path";
                arga = slash == NULL ? NULL : slash + 1;
            } while (arga != NULL);
            paths.push_back(path);
        }

        buffer(~uint64_t(), false, paths);
    } catch (const char *e) {
        std::cerr << e << std::endl;
    }

    std::cout << std::endl;
    return 0;
}
