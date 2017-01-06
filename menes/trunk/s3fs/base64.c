/* Copyright, License, & Warranty Disclaimer {{{ */
/* S3FS - Linux VFS Module for Amazon S3
 * Copyright (C) 2006  Jay Freeman (saurik)
*/

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
/* Include Directives {{{ */
#ifdef __KERNEL__
#include <linux/types.h>
#include <linux/kernel.h>
#else
#include <stdint.h>
#include <stdlib.h>
#endif

#include <stdbool.h>

#include "base64.h"
/* }}} */

static const char Characters_[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void base64_encode(char *output, const uint8_t *data, size_t size) {
    size_t k;
    while (size != 0) {
        size_t remain = size < 3 ? size : 3;
        uint8_t next[3] = { 0, 0, 0 };
        for (k = 0; k != remain; ++k)
            next[k] = *(data++);
        size -= remain;
        *(output++) = Characters_[next[0] >> 2];
        *(output++) = Characters_[(next[0] & 0x03) << 4 | next[1] >> 4];
        if (remain == 1) {
            *(output++) = '=';
            *(output++) = '=';
            break;
        } else {
            *(output++) = Characters_[(next[1] & 0x0f) << 2 | next[2] >> 6];
            if (remain == 3)
                *(output++) = Characters_[next[2] & 0x3f];
            else {
                *(output++) = '=';
                break;
            }
        }
    }
}
