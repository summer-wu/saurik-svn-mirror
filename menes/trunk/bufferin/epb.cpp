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

#include <iostream>
#include <sstream>

int last_(EOF);

uint8_t byte() {
    int c;

    if (last_ != EOF) {
        c = last_;
        last_ = EOF;
    } else {
        c = getchar();
        //fprintf(stderr, "%c", c);
        if (c == EOF)
            throw "early end-of-file";
    }

    return c;
}

void push(uint8_t c) {
    if (last_ != EOF)
        throw "pushing too hard";
    last_ = c;
}

uint64_t number() {
    uint64_t value(0);
    for (;;) {
        uint8_t next(byte());
        if (next < '0' || next > '9') {
            push(next);
            return value;
        }
        value = value * 10 + next - '0';
    }
}

void varint(std::streambuf &out, uint64_t number) {
    do {
        uint8_t now(number & 0x7f);
        number >>= 7;
        if (number != 0)
            now |= 0x80;
        out.sputc(now);
    } while (number != 0);
}

void key(std::streambuf &out, uint64_t tag, uint64_t type) {
    varint(out, (tag << 3) | type);
}

void length(std::streambuf &out, uint64_t tag, std::stringbuf &data) {
    key(out, tag, 2);
    const std::string &value(data.str());
    varint(out, value.size());
    size_t writ(out.sputn(value.data(), value.size()));
    if (writ != value.size())
        throw "premature eof";
}

#define _trace() fprintf(stderr, "_trace():%s(%u)\n", __FILE__, __LINE__)

void buffer(std::streambuf &out, uint8_t end) {
    bool comma(false);

    for (;;) {
        uint8_t next;

        do try {
            next = byte();
        } catch (const char *e) {
            if (end == 0)
                return;
            else
                throw e;
        } while (std::isspace(next));

        if (next == end)
            return;
        if (next == '#')
            exit(0);

        if (!comma) {
            comma = true;
            push(next);
        } else if (next != ',')
            throw "missing ,";

        uint64_t tag(number());
        if (byte() != ':')
            throw "missing :";

        next = byte();
        if (next == '{') {
            key(out, tag, 3);
            buffer(out, '}');
            key(out, tag, 4);
        } else if (next == '[') {
            std::stringbuf data;
            buffer(data, ']');
            length(out, tag, data);
        } else if (next == '\"' || next == '\'') {
            uint8_t start(next);
            std::stringbuf data;
            for (;;) {
                next = byte();
                if (next == start)
                    break;
                if (next == '\\') {
                    next = byte();
                    if (next == 'n')
                        next = '\n';
                    else if (next == 'r')
                        next = '\r';
                    else if (next == 't')
                        next = '\t';
                    else if (next == 'x') {
                        char hex[3];
                        hex[0] = byte();
                        hex[1] = byte();
                        hex[2] = '\0';
                        char *stop;
                        next = strtoul(hex, &stop, 16);
                        if (hex + 2 != stop)
                            throw "invalid encoding";
                    }
                }
                data.sputc(next);
            }
            length(out, tag, data);
        } else if (next < '0' || next > '9')
            throw "unknown encoding";
        else {
            push(next);
            key(out, tag, 0);
            varint(out, number());
        }
    }
}

int main() {
    try {
        buffer(*std::cout.rdbuf(), 0);
    } catch (const char *e) {
        std::cerr << e << std::endl;
    }
    return 0;
}
