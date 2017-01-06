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
#include "platform.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/ipv6.h>
#include <linux/crypto.h>
#include <linux/scatterlist.h>
#include <linux/ctype.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/random.h>
#include <linux/buffer_head.h>
//#include <asm/uaccess.h>

#include <stdbool.h>

#include "base64.h"
#include "udfdecl.h"
/* }}} */
/* Configuration Constants {{{ */
#define HTTP_MAXLINE 1024
#define HTTP_MINLINE 32
#define S3FS_MAGIC 0x53334653
/* }}} */
/* Debug Logging {{{ */
#if 1
#define log(type, format, args ...) \
    printk(KERN_ ## type "s3fs: " format "\n", ## args)
#else
#define log(type, format, args ...)
#endif

#define _trace() do { \
    log(EMERG, "(%s:%u) %s()", __FILE__, __LINE__, __FUNCTION__); \
    /*mdelay(2000);*/ \
} while (false)

#if 1
#define enter_(format, args ...) do { \
    log(EMERG, "(%s:%u) +%s(" format ")", __FILE__, __LINE__, __FUNCTION__, ## args); \
    /*mdelay(2000);*/ \
} while (false)
#else
#define enter_(format, args ...)
#endif
/* }}} */

#define AmazonHost_ "207.171.181.148"
#define AmazonPort_ 80

/* Data Structure: String {{{ */
struct String_ {
    char *data_;
    size_t size_;
};

static _finline void string_movenext_(struct String_ *string) {
    ++string->data_;
    --string->size_;
}

static void string_copy_range_(struct String_ *string, const char *value, size_t size) {
    string->data_ = kmalloc(size, GFP_KERNEL) + 1;
    memcpy(string->data_, value, size);
    string->data_[size] = '\0';
    string->size_ = size;
}

static _finline void string_copy_cstr_(struct String_ *string, const char *value) {
    string_copy_range_(string, value, strlen(value));
}

static _finline void string_copy_string_(struct String_ *string, struct String_ *value) {
    string_copy_range_(string, value->data_, value->size_);
}

static bool string_equals_range_(struct String_ *string, const char *value, size_t size) {
    return
        string->size_ == size && (
            string->data_ == value ||
            memcmp(string->data_, value, size) == 0
        );
}

static _finline void string_export_(char *data, struct String_ *string) {
    memcpy(data, string->data_, string->size_);
}

static void string_lower_(struct String_ *value) {
    size_t i;
    for (i = 0; i != value->size_; ++i)
        value->data_[i] = tolower(value->data_[i]);
}

/*static _finline bool string_empty_(struct String_ *value) {
    return value->size_ == 0;
}*/

#define string_equals_(string, value) \
    string_equals_range_(string, value, sizeof(value) - 1)
/* }}} */
/* Data Structure: Uuid {{{ */
typedef union {
    uint8_t bytes_[16];

    struct {
        uint32_t data1_;
        uint16_t data2_;
        uint16_t data3_;
        uint16_t data4_;
        uint16_t data5_;
        uint32_t data6_;
    } parts_;
} uuid_t;

#define UuidLength_ 36

static void uuid_create_(uuid_t *_this) {
    /* XXX: implement */
    generate_random_uuid(_this->bytes_);
}

static _finline void uuid_unparse_(char *string, uuid_t *uuid) {
    snprintf(string, 37, "%.8x-%.4x-%.4x-%.4x-%.4x%.8x",
        cpu_to_be32(uuid->parts_.data1_),
        cpu_to_be16(uuid->parts_.data2_),
        cpu_to_be16(uuid->parts_.data3_),
        cpu_to_be16(uuid->parts_.data4_),
        cpu_to_be16(uuid->parts_.data5_),
        cpu_to_be32(uuid->parts_.data6_)
    );
}

static _finline void uuid_copy_(uuid_t *_this, uuid_t *value) {
    memmove(_this, value, sizeof(uuid_t));
}

static _finline int uuid_compare_(uuid_t *_this, uuid_t *value) {
    return memcmp(_this, value, sizeof(uuid_t));
}
/* }}} */

/* XXX: move Uuid out and promote this header */
#include "s3data.h"

/* XXX: this is probably bogus */
typedef pgoff_t blkoff_t;

static _finline void dec2_unparse_(char *data, unsigned value) {
    data[0] = value / 10 + '0';
    data[1] = value % 10 + '0';
}

struct Combined_ {
    const char *device_;
    void *data_;
};

struct Connection_ {
    struct Connection_ *next_;
    struct ClientSuper_ *cs_;

    struct socket *socket_;
    struct sockaddr_in address_;

    char *buffer_;
    size_t size_;
};

struct ClientSuper_ {
    struct semaphore lock_;
    struct Connection_ *connection_;

    uint8_t buffer_[4096];
    size_t offset_;
    size_t size_;

    struct String_ host_;
    uint16_t port_;

    struct String_ id_;
    struct String_ key_;

    struct String_ bucket_;
    struct String_ prefix_;

    uint64_t transferred_;
};

struct ClientIndex_ {
    struct inode kindex_;
    uuid_t uuid_;

    struct ServerIndex_ pfile_;
    bool written_;
};

struct crypto_tfm *tfm_;
size_t hsize_ = 512;

/* String Constants {{{ */
#define STR_DEFINE(value) { \
    .data_ = (value), \
    .size_ = sizeof(value) - 1 \
}

struct String_ XmlCode_ = STR_DEFINE("Code");
struct String_ XmlKey_ = STR_DEFINE("Key");

struct String_ HttpDelete_ = STR_DEFINE("DELETE");
struct String_ HttpGet_ = STR_DEFINE("GET");
struct String_ HttpPut_ = STR_DEFINE("PUT");

struct String_ Day_[] = {
    STR_DEFINE("Wed"),
    STR_DEFINE("Thu"),
    STR_DEFINE("Fri"),
    STR_DEFINE("Sat"),
    STR_DEFINE("Sun"),
    STR_DEFINE("Mon"),
    STR_DEFINE("Tue")
};

struct String_ Month_[] = {
    STR_DEFINE("Dec"),
    STR_DEFINE("Jan"),
    STR_DEFINE("Feb"),
    STR_DEFINE("Mar"),
    STR_DEFINE("Apr"),
    STR_DEFINE("May"),
    STR_DEFINE("Jun"),
    STR_DEFINE("Jul"),
    STR_DEFINE("Aug"),
    STR_DEFINE("Sep"),
    STR_DEFINE("Oct"),
    STR_DEFINE("Nov")
};
/* }}} */
/* Character Sets {{{ */
static _finline bool is_newline_(char value) {
    return value == '\r' || value == '\n';
}

static _finline bool is_white_(char value) {
    return value == ' ' || value == '\t';
}
/* }}} */
/* Scatter/Gather Helper Macros {{{ */
#define IOV_RANGE(data, size) \
    do { \
        iov[iovs].iov_base = (data); \
        iov[iovs].iov_len = (size); \
        ++iovs; \
    } while(false)

#define SG_RANGE(data, size) \
    do { \
        /*size_t _i; \
        printk(KERN_INFO "sign: "); \
        for (_i = 0; _i != (size); ++_i) \
            printk(" %.2x", (data)[_i]); \
        printk("\n");*/ \
        sg_init_one(&sg[sgs++], data, size); \
    } while(false)

#define STRING_RANGE_(singular, plural, data, size) \
    do { \
        singular[plural].data_ = (char *) (data); \
        singular[plural].size_ = (size); \
        ++plural; \
    } while (false)

#define URL_RANGE(data, size) \
    STRING_RANGE_(url, urls, data, size)

#define QUERY_RANGE(data, size) \
    STRING_RANGE_(query, queries, data, size)

#define ADD_RANGE(type, data, size) \
    type ## _RANGE(data, size)

#define ADD_CONST(type, string) \
    type ## _RANGE(string, sizeof(string) - 1)

#define ADD_STRING(type, string) \
    type ## _RANGE((string)->data_, (string)->size_)

#define ADD_MULTI(type, string, strings) \
    do { \
        size_t _i; \
        for (_i = 0; _i != (strings); ++_i) \
            ADD_STRING(type, &(string)[_i]); \
    } while (false)
/* }}} */
/* Dump Diagnostics {{{ */
static char simplify_(uint8_t value) {
    return value < 0x20 || value >= 0x80 ? '.' : value;
}

static void dump_(const uint8_t *data, size_t size) {
    size_t i = 0, j;

    while (i != size) {
        if (i % 16 == 0)
            printk(KERN_INFO "0x%.3x:", i);

        if (i % 4 == 0 && i % 16 != 0)
            printk(" ");

        printk(" %.2x", data[i]);

        if (++i % 16 == 0) {
            printk("  ");
            for (j = i - 16; j != i; ++j)
                printk("%c", simplify_(data[j]));
            printk("\n");
        }
    }

    if (i % 16 != 0) {
        printk("   " + ((i % 16) - 1) / 4);
        for (j = i; j % 16 != 0; ++j)
            printk("   ");
        printk("  ");
        for (j = i - i % 16; j != i; ++j)
            printk("%c", simplify_(data[j]));
        printk("\n");
    }
}
/* }}} */

/* Simplistic Xml Parser {{{ */
enum ParseState_ {
    Parse_,
    ParsePi_,
    ParseOpen_,
    ParseClose_,
    ParseTag_,
    ParseText_
};

struct ParseFrame_ {
    enum ParseState_ state_;
    struct String_ *name_;
    bool content_;

    int (*on_content_)(void *, uint8_t *, size_t);
    void *arg_;
};

static size_t on_parse_(void *arg, uint8_t *data, size_t size, bool last) {
    int error = 0;

    struct ParseFrame_ *pframe = (struct ParseFrame_ *) arg;
    size_t offset = 0;

    /*log(INFO, "on_parse_() %u", size);
    dump_(data, size);*/

    while (offset != size)
        switch (pframe->state_) {
            case Parse_: {
                if (data[offset] != '<')
                    pframe->state_ = ParseText_;
                else {
                    ++offset;
                    pframe->state_ = ParseTag_;
                }
            } break;

            case ParseTag_: {
                switch (data[offset]) {
                    case '?':
                        ++offset;
                        pframe->state_ = ParsePi_;
                    break;

                    case '/':
                        ++offset;
                        pframe->state_ = ParseClose_;
                    break;

                    default:
                        pframe->state_ = ParseOpen_;
                    break;
                }
            } break;

            case ParsePi_:
            case ParseClose_: {
                uint8_t *gt = memchr(data + offset, '>', size - offset);
                if (gt == NULL)
                    return size;

                offset = gt - data + 1;
                pframe->state_ = Parse_;
            } break;

            case ParseOpen_: {
                uint8_t *gt = memchr(data + offset, '>', size - offset);
                if (gt == NULL)
                    return offset;

                if (string_equals_range_(pframe->name_, data + offset, gt - data - offset))
                    pframe->content_ = true;


                offset = gt - data + 1;
                pframe->state_ = Parse_;
            } break;

            case ParseText_: {
                uint8_t *lt = memchr(data + offset, '<', size - offset);
                if (lt == NULL) {
                    if (_unlikely(pframe->content_))
                        return offset;
                    else
                        return size;
                }

                if (_unlikely(pframe->content_)) {
                    pframe->content_ = false;
                    _check(pframe->on_content_(pframe->arg_, data + offset, lt - data - offset));
                }

                offset = lt - data + 1;
                pframe->state_ = ParseClose_;
            } break;
        }

    return size;

  error:
    return error;
}
/* }}} */
/* Raw Data Parser {{{ */
struct WriteFrame_ {
    struct String_ *string_;
};

static size_t on_write_(void *arg, uint8_t *data, size_t size, bool last) {
    struct WriteFrame_ *wframe = (struct WriteFrame_ *) arg;
    size_t length;

    if (size <= wframe->string_->size_)
        length = size;
    else {
        log(ERR, "too much data in HTTP body");
        length = wframe->string_->size_;
    }

    memmove(wframe->string_->data_, data, length);

    wframe->string_->data_ += length;
    wframe->string_->size_ -= length;

    return size;
}
/* }}} */

/* S3: Connection Pooling {{{ */
void destroy_connection_(struct Connection_ *connection) {
    if (connection->socket_ != NULL)
        sock_release(connection->socket_);
    if (connection->buffer_ != NULL)
        kfree(connection->buffer_);
    kfree(connection);
}

static void s3_disconnect_(struct ClientSuper_ *cs, struct Connection_ *connection) {
    down(&cs->lock_);

    connection->next_ = cs->connection_;
    cs->connection_ = connection;

    up(&cs->lock_);
}

/* XXX: this function isn't very robust to network outages */
static int s3_connect_(struct ClientSuper_ *cs, struct Connection_ **connection) {
    int error = 0;
    jiffies_t mark;

    down(&cs->lock_);

    if (cs->connection_) {
        *connection = cs->connection_;
        cs->connection_ = (*connection)->next_;
    }

    up(&cs->lock_);

    if (*connection == NULL) {
        *connection = kzalloc(sizeof(struct Connection_), GFP_KERNEL);
        if (*connection == NULL)
            _raise(ENOMEM);

        (*connection)->buffer_ = kmalloc(hsize_, GFP_KERNEL);
        if ((*connection)->buffer_ == NULL)
            _raise(ENOMEM);

        (*connection)->size_ = hsize_;
        (*connection)->cs_ = cs;
        (*connection)->socket_ = NULL;

        _check(sock_create_kern(PF_INET, SOCK_STREAM, IPPROTO_TCP, &(*connection)->socket_));

        (*connection)->address_.sin_family = AF_INET;
        (*connection)->address_.sin_addr.s_addr = in_aton(cs->host_.data_);
        (*connection)->address_.sin_port = htons(cs->port_);

        log(INFO, "connecting...");

        mark = jiffies;

        /* XXX: if this blocks, it blocks _hard_ */
        if ((error = (*connection)->socket_->ops->connect(
            (*connection)->socket_,
            (struct sockaddr *) &(*connection)->address_,
            sizeof((*connection)->address_),
            0
        )) < 0)
            goto error;

        log(INFO, "connect: %lums", (jiffies - mark) * 1000 / HZ);
    }

    return 0;

  error:
    destroy_connection_(*connection);
    *connection = NULL;

    return error;
}
/* }}} */
/* S3: Network Helpers {{{ */
static int s3_send_iov_(struct Connection_ *connection, struct kvec *iov, size_t iovs) {
    int error = 0;
    struct msghdr header;

    size_t total = 0, i;

    for (i = 0; i != iovs; ++i)
        total += iov[i].iov_len;

    header.msg_name = &connection->address_;
    header.msg_namelen = sizeof(connection->address_);
    header.msg_control = NULL;
    header.msg_controllen = 0;
    header.msg_flags = MSG_NOSIGNAL;

    _forever {
        _check(kernel_sendmsg(connection->socket_, &header, iov, iovs, total));
        connection->cs_->transferred_ += error;
        total -= error;

        if (total == 0)
            return 0;

        do if (iov[0].iov_len > error) {
            iov[0].iov_base += error;
            iov[0].iov_len -= error;
            break;
        } else {
            error -= iov[0].iov_len;
            ++iov;
            --iovs;
        } while (error != 0);
    }

  error:
    return error;
}

static int s3_send_string_(struct Connection_ *connection, struct String_ *string) {
    struct kvec iov[1];
    size_t iovs = 0;

    ADD_STRING(IOV, string);
    return s3_send_iov_(connection, iov, iovs);
}

static int s3_receive_(struct Connection_ *connection, uint8_t *data, size_t size) {
    int error = 0;

    struct kvec iov = {
        .iov_base = data,
        .iov_len = size
    };

    struct msghdr header;
    header.msg_flags = MSG_NOSIGNAL;

    _check(kernel_recvmsg(connection->socket_, &header, &iov, 1, size, header.msg_flags));

    /*log(INFO, "s3_receive_()");
    dump_(data, error);*/

    return error;

  error:
    return error;
}
/* }}} */
/* S3: Headers/Authorization {{{ */
static int s3_headers_(
    struct Connection_ *connection,
    const struct String_ *method,
    const struct String_ *url,
    size_t urls,
    const struct String_ *query,
    size_t queries,
    unsigned length
) {
    struct ClientSuper_ *cs;

    struct kvec iov[12 + urls + queries];
    size_t iovs = 0;

    struct scatterlist sg[7 + urls];
    size_t sgs = 0;

    struct timespec now;
    kernel_timestamp timestamp;
    int32_t timezone;

    char content[20 + 1];

    unsigned keysize;
    char digest[20];
    char signature[1 + 28 + 1];

    char date[] = "Day, ## Mon YEAR HH:MM:SS +0000";
                 /*0123456789012345678901234567890*/

    cs = connection->cs_;
    _assert(cs != NULL);

    now = current_kernel_time();
    udf_time_to_stamp(&timestamp, now);
    timezone = -((int32_t) ((0x1000 - (timestamp.typeAndTimezone & 0xfff)) << 4) >> 4);

    string_export_(date + 0, &Day_[now.tv_sec / (24 * 60 * 60) % 7]);
    dec2_unparse_(date + 5, timestamp.day);
    string_export_(date + 8, &Month_[timestamp.month]);

    /* XXX: this is not year 10000 safe */
    dec2_unparse_(date + 12, timestamp.year / 100);
    dec2_unparse_(date + 14, timestamp.year % 100);
    dec2_unparse_(date + 17, timestamp.hour);
    dec2_unparse_(date + 20, timestamp.minute);
    dec2_unparse_(date + 23, timestamp.second);

    if (timezone < 0) {
        date[26] = '-';
        timezone = -timezone;
    }

    dec2_unparse_(date + 27, timezone / 60);
    dec2_unparse_(date + 29, timezone % 60);

    ADD_STRING(SG, method);
    ADD_CONST(SG, "\n");
    ADD_CONST(SG, "\n");
    ADD_CONST(SG, "\n");
    ADD_CONST(SG, date);
    ADD_CONST(SG, "\n");

    ADD_STRING(IOV, method);
    ADD_CONST(IOV, " /");
    ADD_CONST(SG, "/");

    ADD_MULTI(IOV, url, urls);
    ADD_MULTI(SG, url, urls);

    if (queries != _not(size_t)) {
        _assert(query != NULL);
        ADD_CONST(IOV, "?");
        ADD_MULTI(IOV, query, queries);
    }

    keysize = cs->key_.size_;
    crypto_hmac(tfm_, cs->key_.data_, &keysize, sg, sgs, digest);

    signature[0] = ':';
    base64_encode(signature + 1, digest, sizeof(digest));
    signature[sizeof(signature) - 1] = '\0';

    ADD_CONST(IOV, " HTTP/1.1\r\nHost: s3.amazonaws.com\r\nDate: ");
    ADD_CONST(IOV, date);
    ADD_CONST(IOV, "\r\nAuthorization: AWS ");
    ADD_STRING(IOV, &cs->id_);
    ADD_CONST(IOV, signature);
    ADD_CONST(IOV, "\r\n");

    if (length != _not(size_t)) {
        size_t size = sprintf(content, "%u", length);

        ADD_CONST(IOV, "Content-Length: ");
        ADD_RANGE(IOV, content, size);
        ADD_CONST(IOV, "\r\n");
    }

    ADD_CONST(IOV, "\r\n");

    return s3_send_iov_(connection, iov, iovs);
}
/* }}} */
/* S3: Request Generator/Parser {{{ */
typedef size_t (*OnWrite_)(void *arg, uint8_t *data, size_t size, bool last);

struct CodeFrame_ {
    char error_[32];
};

static int on_code_(void *arg, uint8_t *data, size_t size) {
    struct CodeFrame_ *cframe = (struct CodeFrame_ *) arg;

    _assert(data != NULL);
    _assert(cframe != NULL);

    data[size] = '\0';
    strncpy(cframe->error_, data, sizeof(cframe->error_) - 1);
    cframe->error_[sizeof(cframe->error_) - 1] = '\0';

    return 0;
}

enum Delimeter_ {
    Unknown_,
    TransferEncodingChunked_,
    ContentLength_,
    Close_
};

static int s3_increase_(
    struct Connection_ *connection,
    char **buffer,
    size_t *size,
    size_t *offset,
    size_t *begin,
    size_t *end
) {
    int error = 0;

    _assert(connection != NULL);
    _assert(buffer != NULL);
    _assert(*buffer != NULL);
    _assert(size != NULL);
    _assert(offset != NULL);
    _assert(begin != NULL);
    _assert(end != NULL);

    if (_unlikely(*size - *end < HTTP_MINLINE)) {
        if (_unlikely(*size - *end + *offset < HTTP_MINLINE)) {
            char *temp = kmalloc(*size *= 2, GFP_KERNEL);
            if (temp == NULL)
                _raise(ENOMEM);

            memcpy(temp, *buffer + *offset, *end - *offset);

            kfree(*buffer);
            *buffer = temp;
        } else {
            memmove(*buffer, *buffer + *offset, *end - *offset);
        }

        *end -= *offset;
        *begin -= *offset;
        *offset = 0;
    }

    _check(s3_receive_(connection, *buffer + *end, *size - *end));
    *end += error;
    return error;

  error:
    return error;
}

static int s3_request__(
    struct ClientSuper_ *cs,
    struct String_ *method,
    struct String_ *url, size_t urls,
    struct String_ *query, size_t queries,
    struct String_ *content,
    OnWrite_ on_write, void *arg
) {
    int error = 0;

    struct Connection_ *connection = NULL;

    char *buffer = NULL;
    size_t size = HTTP_MAXLINE;
    size_t offset, begin = 0, end = 0;

    enum Delimeter_ delimeter = Unknown_;
    bool close = false;
    unsigned code = _not(unsigned);

    bool truncated = false;
    char *lf = NULL, *colon;
    size_t remain = _not(size_t);

    struct CodeFrame_ cframe;

    /* XXX: move initialization? */
    struct ParseFrame_ pframe = {
        .state_ = Parse_,
        .name_ = &XmlCode_,
        .content_ = false,
        .on_content_ = &on_code_,
        .arg_ = (void *) &cframe
    };

    _assert(cs != NULL);
    _assert(method != NULL);

    _check(s3_connect_(cs, &connection));
    _assert(connection != NULL);

    code = _not(unsigned);

    _check(s3_headers_(
        connection,
        method,
        url, urls,
        query, queries,
        content == NULL ?
            _not(size_t) :
            content->size_
    ));

    if (content != NULL)
        _check(s3_send_string_(connection, content));

    buffer = kmalloc(size, GFP_KERNEL);
    if (buffer == NULL)
        _raise(ENOMEM);

  receive:
    if (_unlikely(size - end < HTTP_MINLINE)) {
        if (_unlikely(truncated)) truncated: {
            begin = 0;
            end = 0;
        } else if (_likely(size - end + begin >= HTTP_MINLINE)) {
            memmove(buffer, buffer + begin, end - begin);
            end = end - begin;
            begin = 0;
        } else if (_unlikely(code == _not(unsigned))) {
            log(ERR, "truncated response code!");
            truncated = true;
            goto code;
        } else {
            truncated = true;
            lf = buffer + end - 1;
            goto parse;
        }
    }

    _check(s3_receive_(connection, buffer + end, size - end));
    if (_unlikely(error == 0)) {
        log(ERR, "premature clean disconnection!");
        _raise(EINVAL);
    }

  line:
    lf = memchr(buffer + end, '\n', error);
    if (lf == NULL) {
        end += error;
        goto receive;
    }

    if (_unlikely(truncated))
        truncated = false;
    else if (_unlikely(code == _not(unsigned))) code: {
        if (_unlikely(memcmp(buffer + begin, "HTTP/1.1 ", 9) != 0)) {
            log(ERR, "protocol not HTTP/1.1");
            _raise(EINVAL);
        }

        code = simple_strtoul(buffer + begin + 9, NULL, 10);
    } else if (_unlikely(is_newline_(buffer[begin]))) {
        begin = lf - buffer + 1;
        end += error;
        goto body;
    } else parse: if (_likely(!is_white_(buffer[begin]))) {
        struct String_ name, value;

        colon = memchr(buffer + begin, ':', lf - buffer - begin);
        if (_unlikely(colon == NULL)) {
            if (_unlikely(truncated))
                log(ERR, "truncated header name!");
            else
                log(ERR, "malformed header line!");

            _raise(EINVAL);
        }

        name.data_ = buffer + begin;
        name.size_ = colon - buffer - begin;

        value.data_ = colon + 1;
        value.size_ = lf - colon - 1;

        while (
            value.size_ != 0 &&
            is_white_(value.data_[0])
        )
            string_movenext_(&value);

        if (value.data_[value.size_ - 1] == '\r')
            --value.size_;

        string_lower_(&name);
        name.data_[name.size_] = '\0';
        value.data_[value.size_] = '\0';

        if (string_equals_(&name, "content-encoding")) {
            log(ERR, "Content-Encoding unsupported!");
            _raise(EINVAL);
        } else if (string_equals_(&name, "connection")) {
            if (string_equals_(&value, "close"))
                close = true;
            else {
                log(ERR, "unknown Connection!");
                _raise(EINVAL);
            }
        } else if (string_equals_(&name, "content-length")) {
            delimeter = ContentLength_;
            remain = simple_strtoul(value.data_, NULL, 10);
        } else if (string_equals_(&name, "transfer-encoding")) {
            if (string_equals_(&value, "chunked"))
                delimeter = TransferEncodingChunked_;
            else {
                log(ERR, "unknown Transfer-Encoding!");
                _raise(EINVAL);
            }
        }

        /*log(INFO, "parsed header:");
        dump_(name.data_, name.size_);
        dump_(value.data_, value.size_);*/
    }

    if (_unlikely(truncated))
        goto truncated;

    begin = lf - buffer + 1;
    error -= begin - end;
    end = begin;
    goto line;

  body:
    offset = begin;

    if (code == 204)
        goto finish;
    else if (_unlikely(code != 200)) {
        cframe.error_[0] = '\0';
        on_write = &on_parse_;
        arg = (void *) &pframe;
    }

    if (_unlikely(delimeter == Unknown_)) {
        if (close)
            delimeter = Close_;
        else {
            log(ERR, "unknown content delimiting!");
            _raise(EINVAL);
        }
    }

    if (_unlikely(delimeter == TransferEncodingChunked_)) chunked: {
        lf = memchr(buffer + begin, '\n', end - begin);
        if (_unlikely(lf == NULL)) {
            _check(s3_increase_(connection, &buffer, &size, &offset, &begin, &end));
            if (error == 0)
                goto unexpected;

            goto chunked;
        }

        *lf = '\0';
        if (lf[-1] == '\r')
            lf[-1] = '\0';

        remain = simple_strtoul(buffer + begin, NULL, 16);

        error = lf - buffer - begin + 1;
        memmove(buffer + offset + error, buffer + offset, begin - offset);
        offset += error;
        begin += error;

        if (remain == 0) {
            /* XXX: make this correctly parse content trailers */

          trailer:
            lf = memchr(buffer + begin, '\n', end - begin);
            if (_unlikely(lf == NULL)) {
                _check(s3_increase_(connection, &buffer, &size, &offset, &begin, &end));
                if (error == 0)
                    goto unexpected;

                goto trailer;
            }

            error = lf - buffer - begin + 1;
            memmove(buffer + offset + error, buffer + offset, begin - offset);
            offset += error;
            begin += error;

            goto finish;
        }
    }

    _forever {
        //log(INFO, "%u %u %u %u", size, offset, begin, end);

        if (_unlikely(remain == _not(size_t)))
            error = end - begin;
        else {
            error = (end - begin > remain ? remain : end - begin);
            remain -= error;
        }
        begin += error;

        error = begin - offset;
        if (_likely(on_write != NULL))
            _check(on_write(arg, buffer + offset, error, false));
        offset += error;

        if (remain == 0)
            break;

        _check(s3_increase_(connection, &buffer, &size, &offset, &begin, &end));
        if (_unlikely(error == 0)) {
            if (delimeter == Close_)
                goto destroy;

            goto unexpected;
        }
    }

    if (_unlikely(delimeter == TransferEncodingChunked_)) {
        /* XXX: make this correctly parse chunk extensions */

      extension:
        lf = memchr(buffer + begin, '\n', end - begin);
        if (_unlikely(lf == NULL)) {
            _check(s3_increase_(connection, &buffer, &size, &offset, &begin, &end));
            if (error == 0)
                goto unexpected;

            goto extension;
        }

        error = lf - buffer - begin + 1;
        memmove(buffer + offset + error, buffer + offset, begin - offset);
        offset += error;
        begin += error;

        goto chunked;
    }

    if (_unlikely(end - begin != 0)) {
        log(ERR, "extra, unused HTTP body");
        dump_(buffer + begin, end - begin);
        _raise(EINVAL);
    }

  finish:
    if (_unlikely(close)) destroy:
        destroy_connection_(connection);
    else
        s3_disconnect_(cs, connection);

    connection = NULL;

    if (_likely(on_write != NULL))
        on_write(arg, buffer + offset, end - offset, true);

    switch (code) {
        case 200:
        case 204:
            code = _not(unsigned);
            error = 0;
        break;

        case 400: /* Bad Request */
            _raise(EINVAL);
        break;

        case 403: /* Forbidden */
            error = -EPERM;
        break;

        case 404:
            code = _not(unsigned);
            error = -ENOENT;
        break;

        case 405: /* Method Not Allowed */
        case 409: /* Conflict */
        case 411: /* Length Required */
        case 412: /* Precondition Failed */
        case 416: /* Requested Range Not Satisfiable */
        case 500: /* Internal ServerError */
        default:
            error = -EINVAL;
        break;
    }

    if (code != _not(unsigned))
        log(ERR, "ERROR (%u: %s)", code, cframe.error_);

    kfree(buffer);

    return error;

  unexpected:
    log(ERR, "connection unexpectedly terminated!");
    _raise(EINVAL);

  error:
    if (buffer != NULL)
        kfree(buffer);

    if (connection != NULL)
        destroy_connection_(connection);

    return error;
}

static int s3_request_(
    struct ClientSuper_ *cs,
    struct String_ *method,
    struct String_ *url, size_t urls,
    struct String_ *query, size_t queries,
    struct String_ *content,
    OnWrite_ on_write, void *arg
) {
    int error;
    jiffies_t mark;

    mark = jiffies;

    /* XXX: retry requests from this top-level vantage */

    error = s3_request__(
        cs,
        method,
        url, urls,
        query, queries,
        content,
        on_write, arg
    );

    log(INFO, "%s: %lums", method->data_, (jiffies - mark) * 1000 / HZ);

    return error;
}
/* }}} */

static _finline struct ClientIndex_ *indexnode_(struct inode *ki) {
    return container_of(ki, struct ClientIndex_, kindex_);
}

static int s3_get_block_(struct inode *ki, char *map, blkoff_t index) {
    int error = 0;

    struct ClientIndex_ *ci;
    struct super_block *ks;
    struct ClientSuper_ *cs;

    char uuid[1 + UuidLength_ + 3 + 1];

    char ac_number[16];
    struct String_ s_number;

    struct String_ url[5];
    size_t urls = 0;

    struct String_ body = {
        .data_ = map,
        .size_ = PAGE_CACHE_SIZE
    };

    struct WriteFrame_ wframe = {
        .string_ = &body
    };

    ci = indexnode_(ki);

    ks = ki->i_sb;
    _assert(ks != NULL);

    cs = (struct ClientSuper_ *) ks->s_fs_info;
    _assert(cs != NULL);

    uuid[0] = '/';
    uuid_unparse_(uuid + 1, &ci->uuid_);
    uuid[sizeof(uuid) - 4] = '/';
    uuid[sizeof(uuid) - 3] = 'b';
    uuid[sizeof(uuid) - 2] = ':';
    uuid[sizeof(uuid) - 1] = '\0';

    s_number.data_ = ac_number;
    s_number.size_ = sprintf(ac_number, "%lu", index);

    ADD_STRING(URL, &cs->bucket_);
    ADD_CONST(URL, "/");
    ADD_STRING(URL, &cs->prefix_);

    ADD_CONST(URL, uuid);
    ADD_STRING(URL, &s_number);

    error = s3_request_(
        cs,
        &HttpGet_,
        url, urls,
        NULL, _not(size_t),
        NULL,
        &on_write_, &wframe
    );

    if (error == -ENOENT) {
        memset(map, 0, PAGE_CACHE_SIZE);
        goto done;
    } else _check(error);

    if (_unlikely(body.size_ != 0)) {
        log(ERR, "truncated data block!");
        _raise(EINVAL);
    }

  done:
    return 0;

  error:
    return error;
}

static int s3_put_block_(struct inode *ki, char *map, blkoff_t index) {
    int error = 0;

    struct ClientIndex_ *ci;
    struct super_block *ks;
    struct ClientSuper_ *cs;

    char uuid[1 + UuidLength_ + 3 + 1];

    char ac_number[16];
    struct String_ s_number;

    struct String_ url[5];
    size_t urls = 0;

    struct String_ body = {
        .data_ = map,
        .size_ = PAGE_CACHE_SIZE
    };

    ci = indexnode_(ki);

    ks = ki->i_sb;
    _assert(ks != NULL);

    cs = (struct ClientSuper_ *) ks->s_fs_info;
    _assert(cs != NULL);

    uuid[0] = '/';
    uuid_unparse_(uuid + 1, &ci->uuid_);
    uuid[sizeof(uuid) - 4] = '/';
    uuid[sizeof(uuid) - 3] = 'b';
    uuid[sizeof(uuid) - 2] = ':';
    uuid[sizeof(uuid) - 1] = '\0';

    s_number.data_ = ac_number;
    s_number.size_ = sprintf(ac_number, "%lu", index);

    ADD_STRING(URL, &cs->bucket_);
    ADD_CONST(URL, "/");
    ADD_STRING(URL, &cs->prefix_);

    ADD_CONST(URL, uuid);
    ADD_STRING(URL, &s_number);

    _check(s3_request_(
        cs,
        &HttpPut_,
        url, urls,
        NULL, _not(size_t),
        &body,
        NULL, NULL
    ));

    return 0;

  error:
    return error;
}

struct TruncateFrame_ {
    struct ClientSuper_ *cs_;
    blkoff_t index_;
};

static int on_truncate_(void *arg, uint8_t *data, size_t size) {
    int error = 0;

    struct TruncateFrame_ *tframe = (struct TruncateFrame_ *) arg;

    size_t prefix = tframe->cs_->prefix_.size_ + 1 + UuidLength_ + 3;

    struct String_ url[3];
    size_t urls = 0;

    if (size < prefix) {
        log(ERR, "invalid returned object name");
        _raise(EINVAL);
    }

    data[size] = '\0';
    if (simple_strtoul(data + prefix, NULL, 10) < tframe->index_)
        goto done;

    ADD_STRING(URL, &tframe->cs_->bucket_);
    ADD_CONST(URL, "/");
    ADD_RANGE(URL, data, size);

    _check(s3_request_(
        tframe->cs_,
        &HttpDelete_,
        url, urls,
        NULL, _not(size_t),
        NULL,
        NULL, NULL
    ));

  done:
    return 0;

  error:
    return error;
}

static int s3_truncate_(struct inode *ki, blkoff_t index) {
    int error = 0;

    struct ClientIndex_ *ci;
    struct super_block *ks;
    struct ClientSuper_ *cs;

    char uuid[1 + UuidLength_ + 3 + 1];

    struct String_ url[1];
    size_t urls = 0;

    struct String_ query[3];
    size_t queries = 0;

    struct TruncateFrame_ tframe;

    struct ParseFrame_ pframe = {
        .state_ = Parse_,
        .name_ = &XmlKey_,
        .content_ = false,
        .on_content_ = &on_truncate_,
        .arg_ = (void *) &tframe
    };

    ci = indexnode_(ki);

    ks = ki->i_sb;
    _assert(ks != NULL);

    cs = (struct ClientSuper_ *) ks->s_fs_info;
    _assert(cs != NULL);

    uuid[0] = '/';
    uuid_unparse_(uuid + 1, &ci->uuid_);
    uuid[sizeof(uuid) - 4] = '/';
    uuid[sizeof(uuid) - 3] = 'b';
    uuid[sizeof(uuid) - 2] = ':';
    uuid[sizeof(uuid) - 1] = '\0';

    ADD_STRING(URL, &cs->bucket_);

    ADD_CONST(QUERY, "prefix=");
    ADD_STRING(QUERY, &cs->prefix_);
    ADD_CONST(QUERY, uuid);

    tframe.index_ = index;
    tframe.cs_ = cs;

    _check(s3_request_(
        cs,
        &HttpGet_,
        url, urls,
        query, queries,
        NULL,
        &on_parse_, (void *) &pframe
    ));

    if (pframe.state_ != Parse_) {
        log(ERR, "malformed XML response!");
        _raise(EINVAL);
    }

    return 0;

  error:
    return error;
}

static int s3_get_super_(struct ClientSuper_ *cs, struct ServerSuper_ *ss) {
    int error = 0;

    struct String_ url[3];
    size_t urls = 0;

    struct String_ body = {
        .data_ = (char *) ss,
        .size_ = sizeof(*ss)
    };

    struct WriteFrame_ wframe = {
        .string_ = &body
    };

    _assert(cs != NULL);
    _assert(ss != NULL);

    ADD_STRING(URL, &cs->bucket_);
    ADD_CONST(URL, "/");
    ADD_STRING(URL, &cs->prefix_);

    _check(s3_request_(
        cs,
        &HttpGet_,
        url, urls,
        NULL, _not(size_t),
        NULL,
        &on_write_, &wframe
    ));

    if (_unlikely(body.size_ != 0)) {
        log(ERR, "truncated super node!");
        _raise(EINVAL);
    }

    return 0;

  error:
    return error;
}

static struct address_space_operations address_space_operations_;

static struct file_operations regular_file_operations_;
static struct file_operations directory_file_operations_;

static struct inode_operations index_operations_;

/* Index Node Helpers {{{ */
static int index_test_(struct inode *ki, void *arg) {
    struct ClientIndex_ *ci;
    uuid_t *id;

    enter_("0x%p, 0x%p", ki, arg);
    _assert(ki != NULL);

    ci = indexnode_(ki);
    _assert(ci != NULL);

    id = (uuid_t *) arg;
    _assert(id != NULL);

    return uuid_compare_(&ci->uuid_, id) == 0;
}

static int index_init_(struct inode *ki, void *arg) {
    struct ClientIndex_ *ci;
    uuid_t *id;

    enter_("0x%p, 0x%p", ki, arg);
    _assert(ki != NULL);

    ci = indexnode_(ki);
    _assert(ci != NULL);

    id = (uuid_t *) arg;
    _assert(id != NULL);

    uuid_copy_(&ci->uuid_, id);
    ci->written_ = false;

    return 0;
}

static int index_set_(struct super_block *ks, struct inode *ki, struct ServerIndex_ *si) {
    _assert(ki != NULL);
    _assert(si != NULL);

    ki->i_mode = si->mode_;
    ki->i_rdev = huge_decode_dev(si->device_);

    ki->i_size = si->size_;
    ki->i_nlink = si->links_;

    ki->i_atime.tv_sec = si->accessed_.seconds_;
    ki->i_atime.tv_nsec = si->accessed_.nseconds_;

    ki->i_mtime.tv_sec = si->modified_.seconds_;
    ki->i_mtime.tv_nsec = si->modified_.nseconds_;

    ki->i_ctime.tv_sec = si->created_.seconds_;
    ki->i_ctime.tv_nsec = si->created_.nseconds_;

    ki->i_uid = si->user_;
    ki->i_gid = si->group_;

    ki->i_blocks = si->blocks_;

    return 0;
}

static int index_read_(struct inode *kindex, uuid_t *node) {
    int error = 0;

    struct super_block *ks;
    struct ClientSuper_ *cs;

    struct String_ url[4];
    size_t urls = 0;

    struct ServerIndex_ sindex;

    struct String_ body = {
        .data_ = (uint8_t *) &sindex,
        .size_ = sizeof(sindex)
    };

    struct WriteFrame_ wframe = {
        .string_ = &body
    };

    char uuid[1 + UuidLength_ + 1];

    _assert(kindex != NULL);

    ks = kindex->i_sb;
    _assert(ks != NULL);

    cs = (struct ClientSuper_ *) ks->s_fs_info;
    _assert(cs != NULL);

    uuid[0] = '/';
    uuid_unparse_(uuid + 1, node);
    uuid[sizeof(uuid) - 1] = '\0';

    ADD_STRING(URL, &cs->bucket_);
    ADD_CONST(URL, "/");
    ADD_STRING(URL, &cs->prefix_);

    ADD_CONST(URL, uuid);

    _check(s3_request_(
        cs,
        &HttpGet_,
        url, urls,
        NULL, _not(size_t),
        NULL,
        &on_write_, &wframe
    ));

    if (_unlikely(body.size_ != 0)) {
        log(ERR, "truncated index node!");
        _raise(EINVAL);
    }

    _check(index_set_(ks, kindex, &sindex));

    return 0;

  error:
    return error;
}

static int index_write_(
    struct inode *ki
) {
    int error = 0;

    struct ClientIndex_ *ci;
    struct super_block *ks;
    struct ClientSuper_ *cs;

    struct ServerIndex_ si;

    struct String_ url[4];
    size_t urls = 0;

    char uuid[1 + UuidLength_ + 1];

    struct String_ body = {
        .data_ = (uint8_t *) &si,
        .size_ = sizeof(si)
    };

    enter_("0x%p", ki);
    _assert(ki != NULL);

    ci = indexnode_(ki);
    _assert(ci != NULL);

    ks = ki->i_sb;
    _assert(ks != NULL);

    cs = (struct ClientSuper_ *) ks->s_fs_info;
    _assert(cs != NULL);

    uuid[0] = '/';
    uuid_unparse_(uuid + 1, &ci->uuid_);
    uuid[sizeof(uuid) - 1] = '\0';

    ADD_STRING(URL, &cs->bucket_);
    ADD_CONST(URL, "/");
    ADD_STRING(URL, &cs->prefix_);

    ADD_CONST(URL, uuid);

    si.mode_ = ki->i_mode;
    si.device_ = huge_encode_dev(ki->i_rdev);

    si.size_ = ki->i_size;
    si.links_ = ki->i_nlink;

    si.accessed_.seconds_ = ki->i_atime.tv_sec;
    si.accessed_.nseconds_ = ki->i_atime.tv_nsec;

    si.modified_.seconds_ = ki->i_mtime.tv_sec;
    si.modified_.nseconds_ = ki->i_mtime.tv_nsec;

    si.created_.seconds_ = ki->i_ctime.tv_sec;
    si.created_.nseconds_ = ki->i_ctime.tv_nsec;

    si.user_ = ki->i_uid;
    si.group_ = ki->i_gid;

    si.blocks_ = ki->i_blocks;

    _check(s3_request_(
        cs,
        &HttpPut_,
        url, urls,
        NULL, _not(size_t),
        &body,
        NULL, NULL
    ));

    ci->written_ = true;

    return 0;

  error:
    return error;
}

struct Create_ {
    int mode_;
    dev_t device_;
};

static int index_get_(
    struct super_block *ks,
    uuid_t *id,
    struct inode **pki,
    struct Create_ *create
) {
    int error = 0;

    struct inode *ki;
    struct ClientIndex_ *ci;

    ino_t hash;

    enter_("0x%p, 0x%p, 0x%p, 0x%p", ks, id, pki, create);
    _assert(ks != NULL);
    _assert(id != NULL);
    _assert(pki != NULL);

    *pki = NULL;
    hash = id->parts_.data1_;

    *pki = iget5_locked(ks, hash, &index_test_, &index_init_, (void *) id);
    if (*pki == NULL)
        _raise(EINVAL);

    ki = *pki;
    ci = indexnode_(ki);

    if ((ki->i_state & I_NEW) != 0) {
        ki->i_ino = hash;

        if (create == NULL) {
            _check(index_read_(ki, id));
            ci->written_ = true;
        } else {
            struct timespec now;

            ki->i_mode = create->mode_;
            ki->i_rdev = create->device_;

            ki->i_size = 0;
            ki->i_nlink = S_ISDIR(ki->i_mode) ? 1 : 0;

            now = current_kernel_time();

            ki->i_atime = now;
            ki->i_mtime = now;
            ki->i_ctime = now;

            /* XXX: where do I get these? */
            ki->i_uid = 0;
            ki->i_gid = 0;

            ki->i_blocks = 0;
        }

        switch (ki->i_mode & S_IFMT) {
            case S_IFREG:
                ki->i_op = &index_operations_;
                ki->i_mapping->a_ops = &address_space_operations_;
                ki->i_fop = &regular_file_operations_;
            break;

            case S_IFDIR:
                ki->i_op = &index_operations_;
                ki->i_mapping->a_ops = &address_space_operations_;
                ki->i_fop = &directory_file_operations_;
            break;

            case S_IFLNK:
                ki->i_op = &index_operations_;
                ki->i_mapping->a_ops = &address_space_operations_;
            break;

            default:
                ki->i_op = &index_operations_;
            break;
        }

        ki->i_blksize = ks->s_blocksize;
        ki->i_blkbits = ks->s_blocksize_bits;

        unlock_new_inode(ki);

        if (create != NULL)
            mark_inode_dirty(ki);
    }

    return 0;

  error:
    if (*pki != NULL) {
        log(ERR, "this is very scary");
        // XXX: what are we going to do?!?
    }

    return error;
}

static int index_new_(struct super_block *ks, struct inode **pki, int mode, dev_t device) {
    int error = 0;

    struct ClientSuper_ *cs;

    uuid_t id;

    struct Create_ create = {
        .mode_ = mode,
        .device_ = device
    };

    enter_("0x%p, 0x%p, 0%o", ks, pki, mode);
    _assert(ks != NULL);
    _assert(pki != NULL);

    cs = (struct ClientSuper_ *) ks->s_fs_info;
    _assert(cs != NULL);

    uuid_create_(&id);
    _check(index_get_(ks, &id, pki, &create));

    return 0;

  error:
    return error;
}

static int get_flag_(struct inode *ki, char *flag) {
    int error = 0;

    switch (ki->i_mode & S_IFMT) {
        case S_IFBLK:  *flag = 'b'; break;
        case S_IFCHR:  *flag = 'c'; break;
        case S_IFDIR:  *flag = 'd'; break;
        case S_IFIFO:  *flag = 'f'; break;
        case S_IFLNK:  *flag = 'l'; break;
        case S_IFREG:  *flag = 'r'; break;
        case S_IFSOCK: *flag = 's'; break;

        default:
            log(ERR, "invalid inode format");
            _raise(EINVAL);
    }

    return 0;

  error:
    return error;
}

static int index_connect_(
    struct inode *ki_parent,
    struct dentry *ke_child,
    struct inode *ki_child
) {
    int error = 0;

    struct ClientIndex_ *ci_parent;
    struct ClientIndex_ *ci_child;

    struct super_block *ks;
    struct ClientSuper_ *cs;

    struct String_ body;

    char uuid[1 + UuidLength_ + 3 + 1];
    char meta[1 + 2 + UuidLength_ + 1];

    struct String_ url[6];
    size_t urls = 0;

    ci_parent = indexnode_(ki_parent);
    ci_child = indexnode_(ki_child);

    ks = ki_parent->i_sb;
    _assert(ks != NULL);

    cs = (struct ClientSuper_ *) ks->s_fs_info;
    _assert(cs != NULL);

    body.data_ = ci_child->uuid_.bytes_;
    body.size_ = sizeof(ci_child->uuid_);

    uuid[0] = '/';
    uuid_unparse_(uuid + 1, &ci_parent->uuid_);
    uuid[sizeof(uuid) - 4] = '/';
    uuid[sizeof(uuid) - 3] = 'l';
    uuid[sizeof(uuid) - 2] = ':';
    uuid[sizeof(uuid) - 1] = '\0';

    meta[0] = '/';
    _check(get_flag_(ki_child, &meta[1]));
    meta[2] = ':';
    uuid_unparse_(meta + 3, &ci_child->uuid_);
    meta[sizeof(meta) - 1] = '\0';

    ADD_STRING(URL, &cs->bucket_);
    ADD_CONST(URL, "/");
    ADD_STRING(URL, &cs->prefix_);

    ADD_CONST(URL, uuid);

    /* XXX: URL encode! */
    ADD_RANGE(URL, ke_child->d_name.name, ke_child->d_name.len);

    _check(s3_request_(
        cs,
        &HttpPut_,
        url, urls,
        NULL, _not(size_t),
        &body,
        NULL, NULL
    ));

    body.size_ = 0;

    uuid[sizeof(uuid) - 3] = 'r';
    ADD_CONST(URL, meta);

    _check(s3_request_(
        cs,
        &HttpPut_,
        url, urls,
        NULL, _not(size_t),
        &body,
        NULL, NULL
    ));

    inode_inc_link_count(ki_child);

    return 0;

  error:
    return error;
}

static int index_disconnect_(
    struct inode *ki_parent,
    struct dentry *ke_child
) {
    int error = 0;

    struct inode *ki_child;
    struct ClientIndex_ *ci_parent;
    struct ClientIndex_ *ci_child;

    struct super_block *ks;
    struct ClientSuper_ *cs;

    char uuid[1 + UuidLength_ + 3 + 1];
    char meta[1 + 2 + UuidLength_ + 1];

    struct String_ url[6];
    size_t urls = 0;

    ki_child = ke_child->d_inode;
    _assert(ki_child != NULL);

    ci_parent = indexnode_(ki_parent);
    ci_child = indexnode_(ki_child);

    ks = ki_parent->i_sb;
    _assert(ks != NULL);

    cs = (struct ClientSuper_ *) ks->s_fs_info;
    _assert(cs != NULL);

    uuid[0] = '/';
    uuid_unparse_(uuid + 1, &ci_parent->uuid_);
    uuid[sizeof(uuid) - 4] = '/';
    uuid[sizeof(uuid) - 3] = 'l';
    uuid[sizeof(uuid) - 2] = ':';
    uuid[sizeof(uuid) - 1] = '\0';

    meta[0] = '/';
    _check(get_flag_(ki_child, &meta[1]));
    meta[2] = ':';
    uuid_unparse_(meta + 3, &ci_child->uuid_);
    meta[sizeof(meta) - 1] = '\0';

    ADD_STRING(URL, &cs->bucket_);
    ADD_CONST(URL, "/");
    ADD_STRING(URL, &cs->prefix_);

    ADD_CONST(URL, uuid);

    /* XXX: URL encode! */
    ADD_RANGE(URL, ke_child->d_name.name, ke_child->d_name.len);

    _check(s3_request_(
        cs,
        &HttpDelete_,
        url, urls,
        NULL, _not(size_t),
        NULL,
        NULL, NULL
    ));

    uuid[sizeof(uuid) - 3] = 'r';
    ADD_CONST(URL, meta);

    _check(s3_request_(
        cs,
        &HttpDelete_,
        url, urls,
        NULL, _not(size_t),
        NULL,
        NULL, NULL
    ));

    inode_dec_link_count(ki_child);
    return 0;

  error:
    return error;
}
/* }}} */

/* VFS: Index Node Operations {{{ */
static int index_make_(struct inode *ki_parent, struct dentry *ke_child, int mode, dev_t device) {
    int error = 0;

    struct inode *ki_child;
    struct super_block *ks;

    _assert(ki_parent != NULL);
    _assert(ke_child != NULL);

    ks = ki_parent->i_sb;
    _assert(ks != NULL);

    _check(index_new_(ks, &ki_child, mode, device));
    _check(index_connect_(ki_parent, ke_child, ki_child));

    d_instantiate(ke_child, ki_child);

    return 0;

  error:
    if (ki_child != NULL)
        iput(ki_child);

    return error;
}

static int index_create_(struct inode *ki_parent, struct dentry *ke_child, int mode, struct nameidata *data) {
    enter_("0x%p, 0x%p, 0%o, 0x%p", ki_parent, ke_child, mode, data);
    return index_make_(ki_parent, ke_child, mode, 0);
}

static int index_link_(struct dentry *ke_from_child, struct inode *ki_to_parent, struct dentry *ke_to_child) {
    int error = 0;

    struct inode *ki_from_child;

    ki_from_child = ke_from_child->d_inode;
    _assert(ki_from_child != NULL);

    enter_("0x%p, 0x%p, 0x%p", ke_from_child, ki_to_parent, ke_to_child);

    _check(index_connect_(ki_to_parent, ke_to_child, ki_from_child));

    return 0;

  error:
    return error;
}

static int index_mkdir_(struct inode *ki_parent, struct dentry *ke_child, int mode) {
    int error = 0;

    enter_("0x%p, 0x%p, 0%o", ki_parent, ke_child, mode);

    _check(index_make_(ki_parent, ke_child, mode | S_IFDIR, 0));
    inode_inc_link_count(ki_parent);

    return 0;

  error:
    return error;
}

static int index_mknod_(struct inode *ki_parent, struct dentry *ke_child, int mode, dev_t device) {
    enter_("0x%p, 0x%p, 0%o, 0x%x", ki_parent, ke_child, mode, device);
    return index_make_(ki_parent, ke_child, mode, device);
}

static int index_rename_(
    struct inode *ki_from_parent,
    struct dentry *ke_from_child,
    struct inode *ki_to_parent,
    struct dentry *ke_to_child
) {
    int error = 0;

    struct inode *ki_from_child;

    enter_("0x%p, 0x%p, 0x%p, 0x%p", ki_from_parent, ke_from_child, ki_to_parent, ke_to_child);

    ki_from_child = ke_from_child->d_inode;
    _assert(ki_from_child != NULL);

    /* XXX: needs to unlink the old child somehow */
    _check(index_connect_(ki_to_parent, ke_to_child, ki_from_child));
    _check(index_disconnect_(ki_from_parent, ke_from_child));

    return 0;

  error:
    return error;
}

static int index_rmdir_(struct inode *ki_parent, struct dentry *ke_child) {
    int error = 0;

    struct inode *ki_child;

    enter_("0x%p, 0x%p", ki_parent, ke_child);

    ki_child = ke_child->d_inode;
    _assert(ki_child != NULL);

    if (ki_child->i_nlink != 2)
        _raise(ENOTEMPTY);

    _check(index_disconnect_(ki_parent, ke_child));

    inode_dec_link_count(ki_parent);
    inode_dec_link_count(ki_child);

    return 0;

  error:
    return error;
}

/* XXX: if page_symlink() fails we should probably destroy the inode somehow! */
static int index_symlink_(struct inode *ki_parent, struct dentry *ke_child, const char *ac_target) {
    int error = 0;

    struct inode *ki_child;

    size_t length;

    enter_("0x%p, 0x%p, 0x%p", ki_parent, ke_child, ac_target);
    _assert(ki_parent != NULL);
    _assert(ke_child != NULL);
    _assert(ac_target != NULL);

    length = strlen(ac_target) + 1;

    _check(index_make_(ki_parent, ke_child, S_IRWXUGO | S_IFLNK, 0));

    ki_child = ke_child->d_inode;
    _assert(ki_child != NULL);

    _check(page_symlink(ki_child, ac_target, length));

    return 0;

  error:
    return error;
}

static void index_truncate_(struct inode *ki) {
    enter_("0x%p", ki);
}

static int index_unlink_(struct inode *ki_parent, struct dentry *ke_child) {
    enter_("0x%p, 0x%p", ki_parent, ke_child);
    return index_disconnect_(ki_parent, ke_child);
}

static struct dentry *index_lookup_(struct inode *kindex, struct dentry *kentry, struct nameidata *data) {
    int error = 0;

    struct ClientIndex_ *cindex;
    struct super_block *ks;
    struct ClientSuper_ *cs;

    struct String_ url[5];
    size_t urls = 0;

    char uuid[1 + UuidLength_ + 3 + 1];

    uuid_t uchild;
    struct inode *kchild;

    struct String_ body = {
        .data_ = uchild.bytes_,
        .size_ = sizeof(uchild)
    };

    struct WriteFrame_ wframe = {
        .string_ = &body
    };

    enter_("0x%p, 0x%p, 0x%p", kindex, kentry, data);

    cindex = indexnode_(kindex);

    ks = kindex->i_sb;
    _assert(ks != NULL);

    cs = (struct ClientSuper_ *) ks->s_fs_info;
    _assert(cs != NULL);

    uuid[0] = '/';
    uuid_unparse_(uuid + 1, &cindex->uuid_);
    uuid[sizeof(uuid) - 4] = '/';
    uuid[sizeof(uuid) - 3] = 'l';
    uuid[sizeof(uuid) - 2] = ':';
    uuid[sizeof(uuid) - 1] = '\0';

    ADD_STRING(URL, &cs->bucket_);
    ADD_CONST(URL, "/");
    ADD_STRING(URL, &cs->prefix_);

    ADD_CONST(URL, uuid);

    /* XXX: URL encode! */
    ADD_RANGE(URL, kentry->d_name.name, kentry->d_name.len);

    if (_unlikely((error = s3_request_(
        cs,
        &HttpGet_,
        url, urls,
        NULL, _not(size_t),
        NULL,
        &on_write_, &wframe
    )) < 0)) switch (error) {
        case -ENOENT:
            return NULL;
        default:
            goto error;
    }

    if (_unlikely(body.size_ != 0)) {
        log(ERR, "truncated uuid!");
        _raise(EINVAL);
    }

    _check(index_get_(ks, &uchild, &kchild, NULL));

    d_add(kentry, kchild);
    return NULL;

  error:
    return ERR_PTR(error);
}

static struct inode_operations index_operations_ = {
    .create = &index_create_,
    .link = &index_link_,
    .lookup = &index_lookup_,
    .mkdir = &index_mkdir_,
    .mknod = &index_mknod_,
    .rename = &index_rename_,
    .rmdir = &index_rmdir_,
    .symlink = &index_symlink_,
    .truncate = &index_truncate_,
    .unlink = &index_unlink_
};
/* }}} */
/* VFS: File Operations {{{ */
struct DirectoryFrame_ {
    struct file *kfile_;
    filldir_t filldir_;
    void *arg_;
};

static int on_key_(void *arg, uint8_t *data, size_t size) {
    int error = 0;

    struct DirectoryFrame_ *dframe = (struct DirectoryFrame_ *) arg;
    /* XXX: I don't know if I like this */
    struct ClientSuper_ *cs = (struct ClientSuper_ *) dframe->kfile_->f_dentry->d_inode->i_sb->s_fs_info;

    size_t prefix = cs->prefix_.size_ + 1 + UuidLength_ + 3;
    size_t postfix = 3 + UuidLength_;

    unsigned type;

    switch (data[size - postfix + 1]) {
        case 'b': type = DT_BLK; break;
        case 'c': type = DT_CHR; break;
        case 'd': type = DT_DIR; break;
        case 'f': type = DT_SOCK; break;
        case 'l': type = DT_LNK; break;
        case 'r': type = DT_REG; break;
        case 's': type = DT_FIFO; break;

        default:
            /* XXX: DT_UNKNOWN */
            _raise(EINVAL);
        break;
    }

    if (size < prefix + postfix) {
        log(ERR, "invalid returned object name");
        _raise(EINVAL);
    }

    /* XXX: check for eof! */
    if ((error = dframe->filldir_(
        dframe->arg_,
        data + prefix,
        size - prefix - postfix,
        ++dframe->kfile_->f_pos,
        parent_ino(dframe->kfile_->f_dentry),
        type
    )) < 0)
        goto error;

    return 0;

  error:
    return error;
}

static int directory_readdir_(struct file *kf, void *arg, filldir_t filldir) {
    int error = 0;

    struct dentry *ke;
    struct inode *kindex;
    struct ClientIndex_ *cindex;
    struct super_block *ks;
    struct ClientSuper_ *cs;

    char uuid[1 + UuidLength_ + 3 + 1];

    struct String_ url[1];
    size_t urls = 0;

    struct String_ query[3];
    size_t queries = 0;

    struct DirectoryFrame_ dframe = {
        .kfile_ = kf,
        .filldir_ = filldir,
        .arg_ = arg
    };

    struct ParseFrame_ pframe = {
        .state_ = Parse_,
        .name_ = &XmlKey_,
        .content_ = false,
        .on_content_ = &on_key_,
        .arg_ = (void *) &dframe
    };

    enter_("0x%p, 0x%p, 0x%p", kf, arg, filldir);

    _assert(kf != NULL);

    ke = kf->f_dentry;
    _assert(ke != NULL);

    kindex = ke->d_inode;
    _assert(kindex != NULL);

    cindex = indexnode_(kindex);
    _assert(cindex != NULL);

    ks = kindex->i_sb;
    _assert(ks != NULL);

    cs = (struct ClientSuper_ *) ks->s_fs_info;
    _assert(cs != NULL);

    switch (kf->f_pos) {
        /* XXX: obviously wrong! */
      default:
        return 0;
      case 0:
        if ((error = filldir(arg, ".", 1, 0, kindex->i_ino, DT_DIR)) < 0)
            goto error;
      case 1:
        if ((error = filldir(arg, "..", 2, 1, parent_ino(ke), DT_DIR)) < 0)
            goto error;
        kf->f_pos = 2;
    }

    uuid[0] = '/';
    uuid_unparse_(uuid + 1, &cindex->uuid_);
    uuid[sizeof(uuid) - 4] = '/';
    uuid[sizeof(uuid) - 3] = 'r';
    uuid[sizeof(uuid) - 2] = ':';
    uuid[sizeof(uuid) - 1] = '\0';

    ADD_STRING(URL, &cs->bucket_);

    ADD_CONST(QUERY, "prefix=");
    ADD_STRING(QUERY, &cs->prefix_);
    ADD_CONST(QUERY, uuid);

    _check(s3_request_(
        cs,
        &HttpGet_,
        url, urls,
        query, queries,
        NULL,
        &on_parse_, (void *) &pframe
    ));

    if (pframe.state_ != Parse_) {
        log(ERR, "malformed XML response!");
        _raise(EINVAL);
    }

    return 0;

  error:
    return error;
}

static struct file_operations regular_file_operations_ = {
    .aio_read = &generic_file_aio_read,
    .aio_write = &generic_file_aio_write,
    .fsync = &simple_sync_file,
    //.ioctl = &file_ioctl_,
    .llseek = &generic_file_llseek,
    .mmap = &generic_file_mmap,
    .open = &generic_file_open,
    .read = &generic_file_read,
    .readv = &generic_file_readv,
    //.release = &file_release_,
    .sendfile = &generic_file_sendfile,
    .splice_read = &generic_file_splice_read,
    .splice_write = &generic_file_splice_write,
    .write = &generic_file_write,
    .writev = &generic_file_writev
};

static struct file_operations directory_file_operations_ = {
    .readdir = &directory_readdir_
};
/* }}} */
/* VFS: Address Space Operations {{{ */
static void show_page_(struct page *page) {
    /*if (page != NULL)
        log(INFO, "page(0x%p): index=%lu\n", page, page->index);*/
}

static int on_commit_write_(
    struct file *kf,
    struct page *page,
    unsigned from,
    unsigned to
) {
    struct dentry *ke;
    struct inode *ki;

    char *map;

    enter_("0x%p, 0x%p, %u, %u", kf, page, from, to);
    _assert(kf != NULL);
    _assert(page != NULL);

    ke = kf->f_dentry;
    _assert(ke != NULL);

    ki = ke->d_inode;
    _assert(ki != NULL);

    map = kmap(page);
    _assert(map != NULL);

    kunmap(map);

    return 0;
}

static int on_get_block_(struct inode *ki, sector_t block, struct buffer_head *result, int create) {
    int error = 0;

    enter_("0x%p, %u, 0x%p, %u", ki, (unsigned) block, result, create);

    _raise(EINVAL);

    return 0;

  error:
    return error;
}

static int on_prepare_write_(
    struct file *kf,
    struct page *page,
    unsigned from,
    unsigned to
) {
    int error = 0;

    struct dentry *ke;
    struct inode *ki;

    char *map = NULL;

    enter_("0x%p, 0x%p, %u, %u", kf, page, from, to);
    show_page_(page);

    ke = kf->f_dentry;
    _assert(ke != NULL);

    ki = ke->d_inode;
    _assert(ki != NULL);

    if (!page_has_buffers(page))
        create_empty_buffers(page, PAGE_CACHE_SIZE, 0);

    if (!PageUptodate(page)) {
        if (from != 0 || to != PAGE_CACHE_SIZE) {
            map = kmap(page);
            _assert(map != NULL);
            _check(s3_get_block_(ki, map, page->index));
            kunmap(page);
            map = NULL;
        }

        SetPageUptodate(page);
    }

    return 0;

  error:
    if (map != NULL)
        kunmap(page);
    return error;
}

static int on_readpage_(
    struct file *kf,
    struct page *page
) {
    int error = 0;

    char *map = NULL;

    struct dentry *ke;
    struct inode *ki;

    enter_("0x%p, 0x%p", kf, page);
    show_page_(page);

    _assert(kf != NULL);
    _assert(page != NULL);

    ke = kf->f_dentry;
    _assert(ke != NULL);

    ki = ke->d_inode;
    _assert(ki != NULL);

    if (!PageUptodate(page)) {
        map = kmap(page);
        _assert(map != NULL);

        _check(s3_get_block_(ki, map, page->index));

        kunmap(page);
        map = NULL;

        SetPageUptodate(page);
    }

    unlock_page(page);
    return 0;

  error:
    if (map != NULL)
        kunmap(page);
    return error;
}

static int on_writepage_(
    struct page *page,
    struct writeback_control *control
) {
    int error = 0;

    struct inode *ki;

    char *map = NULL;

    enter_("0x%p, 0x%p", page, control);
    show_page_(page);

    _assert(page != NULL);
    _assert(control != NULL);

    ki = page->mapping->host;
    _assert(ki != NULL);

    map = kmap(page);
    _assert(map != NULL);

    _check(s3_put_block_(ki, map, page->index));

    kunmap(page);
    map = NULL;

    unlock_page(page);
    return 0;

  error:
    if (map != NULL)
        kunmap(page);

    set_page_dirty(page);
    unlock_page(page);
    log(ERR, "writepage() failed!");
    return error;
}

static struct address_space_operations address_space_operations_ = {
    .commit_write = &generic_commit_write,
    .prepare_write = &on_prepare_write_,
    .readpage = &on_readpage_,
    //.sync_page = &on_sync_page_,
    .writepage = &on_writepage_
};
/* }}} */
/* VFS: Super Block Operations {{{ */
static kmem_cache_t *index_cache_;

static struct inode *allocate_index_(void) {
    struct ClientIndex_ *cindex;
    enter_("");
    cindex = kmem_cache_alloc(index_cache_, SLAB_KERNEL);
    return &cindex->kindex_;
}

static void initialize_index_(void *arg, kmem_cache_t *cache, unsigned long flags) {
    struct ClientIndex_ *cindex;

    //enter_("0x%p, 0x%p, 0x%lx", arg, cache, flags);
    _assert(cache != NULL);

    cindex = (struct ClientIndex_ *) arg;
    _assert(cindex != NULL);

    if ((flags & (SLAB_CTOR_VERIFY | SLAB_CTOR_CONSTRUCTOR)) == SLAB_CTOR_CONSTRUCTOR)
        inode_init_once(&cindex->kindex_);
}

static struct inode *on_alloc_inode_(struct super_block *ks) {
    struct ClientSuper_ *cs;
    enter_("0x%p", ks);
    cs = (struct ClientSuper_ *) ks->s_fs_info;
    return allocate_index_();
}

static void on_delete_inode_(struct inode *ki) {
    int error = 0;

    struct ClientIndex_ *ci;
    struct super_block *ks;
    struct ClientSuper_ *cs;

    struct String_ url[4];
    size_t urls = 0;

    char uuid[1 + UuidLength_ + 1];

    enter_("0x%p", ki);

    ci = indexnode_(ki);

    if (!ci->written_)
        goto clear;

    ks = ki->i_sb;
    _assert(ks != NULL);

    cs = (struct ClientSuper_ *) ks->s_fs_info;
    _assert(cs != NULL);

    uuid[0] = '/';
    uuid_unparse_(uuid + 1, &ci->uuid_);
    uuid[sizeof(uuid) - 1] = '\0';

    ADD_STRING(URL, &cs->bucket_);
    ADD_CONST(URL, "/");
    ADD_STRING(URL, &cs->prefix_);

    ADD_CONST(URL, uuid);

    _check(s3_request_(
        cs,
        &HttpDelete_,
        url, urls,
        NULL, _not(size_t),
        NULL,
        NULL, NULL
    ));

    _check(s3_truncate_(ki, 0));

  clear:
    truncate_inode_pages(&ki->i_data, 0);
    clear_inode(ki);

    return;

  error:
    log(ERR, "on_delete_inode_() failed!");
    return;
}

static void on_destroy_inode_(struct inode *ki) {
    struct ClientIndex_ *ci;
    enter_("0x%p", ki);
    ci = indexnode_(ki);
    kmem_cache_free(index_cache_, ci);
}

static void on_drop_inode_(struct inode *kindex) {
    struct ClientIndex_ *cindex;
    enter_("0x%p", kindex);
    cindex = indexnode_(kindex);
    generic_delete_inode(kindex);
}

/*static void on_read_inode_(struct inode *kindex) {
    struct ClientIndex_ *cindex;
    enter_("0x%p", kindex);
    cindex = indexnode_(kindex);
    RR
}*/

static int on_write_inode_(struct inode *kindex, int sync) {
    enter_("0x%p, %d", kindex, sync);
    _assert(kindex != NULL);
    return index_write_(kindex);
}

static void on_put_super_(struct super_block *ks) {
    struct ClientSuper_ *cs;
    enter_("0x%p", ks);
    cs = (struct ClientSuper_ *) ks->s_fs_info;
}

static int on_remount_fs_(struct super_block *ks, int *flags, char *data) {
    struct ClientSuper_ *info;
    enter_("0x%p, [0x%p] 0x%x, \"%s\"", ks, flags, *flags, data);
    info = (struct ClientSuper_ *) ks->s_fs_info;
    return -ENOTTY;
}

static int on_show_options_(struct seq_file *sequence, struct vfsmount *mount) {
    enter_("0x%p, 0x%p", sequence, mount);
    return -ENOTTY;
}

static int on_statfs_(struct super_block *ks, struct kstatfs *buf) {
    struct ClientSuper_ *info;
    enter_("0x%p, 0x%p", ks, buf);
    info = (struct ClientSuper_ *) ks->s_fs_info;
    return -ENOTTY;
}

static struct super_operations super_operations_ = {
    .alloc_inode = &on_alloc_inode_,
    .delete_inode = &on_delete_inode_,
    .destroy_inode = &on_destroy_inode_,
    /*.drop_inode = &on_drop_inode_*//*,
    .read_inode = &on_read_inode_,
    .put_super = &on_put_super_,
    .remount_fs = &on_remount_fs_,
    .show_options = &on_show_options_,
    .statfs = &on_statfs_,*/
    .write_inode = &on_write_inode_
};
/* }}} */
/* VFS: File System Operations {{{ */
static void cleanup_super_(struct super_block *ks) {
    struct ClientSuper_ *cs;

    enter_("0x%p", ks);

    cs = (struct ClientSuper_ *) ks->s_fs_info;
    if (cs == NULL)
        return;

    while (cs->connection_ != NULL) {
        struct Connection_ *next = cs->connection_->next_;
        destroy_connection_(cs->connection_);
        cs->connection_ = next;
    }

    kfree(cs);
    ks->s_fs_info = NULL;
}

static bool parse_options_(struct ClientSuper_ *cs, void *data) {
    struct String_ options;

    if (data == NULL)
        return true;

    options.data_ = (char *) data;
    options.size_ = strlen(options.data_);

    while (options.size_ != 0) {
        struct String_ name, value;

        if (*options.data_ == ',') {
            string_movenext_(&options);
            continue;
        }

        name.data_ = options.data_;

        if ((options.data_ = memchr(options.data_, ',', options.size_)) == NULL) {
            name.size_ = options.size_;
            options.size_ = 0;
        } else {
            name.size_ = options.data_ - name.data_;
            options.size_ -= name.size_;
            *options.data_ = '\0';
            string_movenext_(&options);
        }

        if ((value.data_ = memchr(name.data_, '=', name.size_)) == NULL)
            value.size_ = 0;
        else {
            size_t size = name.size_;
            name.size_ = value.data_ - name.data_;

            *(value.data_++) = '\0';
            value.size_ = size - name.size_ - 1;
        }

        if (string_equals_(&name, "id")) {
            if (value.data_ == NULL)
                return false;
            string_copy_string_(&cs->id_, &value);
        } else if (string_equals_(&name, "key")) {
            if (value.data_ == NULL)
                return false;
            string_copy_string_(&cs->key_, &value);
        } else if (string_equals_(&name, "host")) {
            if (value.data_ == NULL)
                return false;
            string_copy_string_(&cs->host_, &value);
        } else if (string_equals_(&name, "port")) {
            if (value.data_ == NULL)
                return false;
            /* XXX: verify number? */
            cs->port_ = simple_strtoul(value.data_, NULL, 0);
        } else return false;
    }

    return true;
}

static int on_fill_super_(struct super_block *ks, void *data, int silent) {
    int error = 0;

    struct Combined_ *combined = (struct Combined_ *) data;
    struct ClientSuper_ *cs;
    struct ServerSuper_ ss;
    struct inode *root = NULL;

    size_t length;
    char *extra, *slash;

    enter_("0x%p, 0x%p, %u", ks, data, silent);

    ks->s_magic = S3FS_MAGIC;
    ks->s_op = &super_operations_;
    ks->s_time_gran = 1;

    snprintf(ks->s_id, sizeof(ks->s_id), "s3");

    length = strlen(combined->device_);

    cs = (struct ClientSuper_ *) kzalloc(sizeof(struct ClientSuper_) + length + 1, GFP_KERNEL);
    if (cs == NULL)
        _raise(ENOMEM);

    ks->s_fs_info = (void *) cs;

    extra = (char *) (cs + 1);

    memcpy(extra, combined->device_, length);
    extra[length] = '\0';

    slash = memchr(extra, '/', length);
    if (slash == NULL)
        _raise(EINVAL);

    cs->bucket_.data_ = extra;
    cs->bucket_.size_ = slash - extra;
    *slash = '\0';

    cs->prefix_.data_ = slash + 1;
    cs->prefix_.size_ = length - cs->bucket_.size_ - 1;

    init_MUTEX(&cs->lock_);

    if (!parse_options_(cs, combined->data_))
        _raise(EINVAL);

    if (cs->host_.size_ == 0)
        string_copy_cstr_(&cs->host_, AmazonHost_);

    if (cs->port_ == 0)
        cs->port_ = AmazonPort_;

    if (cs->id_.size_ == 0)
        _raise(EINVAL);

    if (cs->key_.size_ == 0)
        _raise(EINVAL);

    _check(s3_get_super_(cs, &ss));

    ks->s_blocksize_bits = ss.bits_;
    ks->s_blocksize = 1 << ks->s_blocksize_bits;

    _check(index_get_(ks, &ss.root_, &root, NULL));

    ks->s_root = d_alloc_root(root);
    if (ks->s_root == NULL)
        _raise(ENOMEM);
    root = NULL;

    return 0;

  error:
    if (root != NULL)
        iput(root);

    cleanup_super_(ks);

    return error;
}

static
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 18)
int
#else
struct super_block *
#endif
on_get_super_(
    struct file_system_type *system,
    int flags,
    const char *device,
    void *data
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 18)
    , struct vfsmount *mount
#endif
) {
    struct Combined_ combined = {
        .device_ = device,
        .data_ = data
    };

    enter_("0x%p, 0x%x, \"%s\", 0x%p", system, flags, device, data);

    return get_sb_nodev(system, flags, &combined, &on_fill_super_
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 18)
        , mount
#endif
    );
}

static void on_kill_super_(struct super_block *ks) {
    kill_anon_super(ks);
    cleanup_super_(ks);
}

static struct file_system_type s3fs_ = {
    .owner = THIS_MODULE,
    .next = NULL,
    .name = "s3fs",
    .get_sb = &on_get_super_,
    .kill_sb = &on_kill_super_,
    .fs_flags = 0
};
/* }}} */

/* Module Initialization/Termination {{{ */
static void on_cleanup_(void) {
    enter_("");

    if (index_cache_ != NULL)
        kmem_cache_destroy(index_cache_);
    if (tfm_ != NULL)
        crypto_free_tfm(tfm_);
    unregister_filesystem(&s3fs_);
}

static int __init on_init_(void) {
    int error;

    if ((error = register_filesystem(&s3fs_)) != 0)
        goto error;

    if ((tfm_ = crypto_alloc_tfm("sha1", 0)) == NULL) {
        /* XXX: what to do? */
        _raise(ENOMEM);
    }

    if ((index_cache_ = kmem_cache_create(
        "s3fs_index_cache3",
        sizeof(struct ClientIndex_),
        0,
        SLAB_RECLAIM_ACCOUNT | SLAB_MEM_SPREAD,
        &initialize_index_,
        NULL
    )) == NULL)
        _raise(ENOMEM);

    log(INFO, "installed.");

    return 0;

  error:
    on_cleanup_();

    return error;
}

static void __exit on_exit_(void) {
    on_cleanup_();
}

module_init(on_init_)
module_exit(on_exit_)
/* }}} */
/* Module Configuration {{{ */
MODULE_AUTHOR("Jay Freeman (saurik) <saurik@saurik.com>");
MODULE_DESCRIPTION("Amazon S3 File System");
MODULE_LICENSE("Dual BSD/GPL");
/* }}} */
