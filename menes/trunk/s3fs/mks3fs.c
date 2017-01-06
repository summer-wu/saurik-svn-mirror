#include "platform.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <getopt.h>
#include <openssl/hmac.h>
#include <uuid/uuid.h>

#include <sys/stat.h>

#include "base64.h"
#include "s3data.h"

static const char *argv0_;
static const char *AmazonHost_ = "s3.amazonaws.com";
static uint16_t AmazonPort_ = 80;

size_t on_read_(const uint8_t *buffer, size_t size, size_t count, void *arg) {
    return 0;
}

struct Block_ {
    const uint8_t *data_;
    size_t size_;
};

/* XXX: this ignores count... */
size_t on_write_(uint8_t *buffer, size_t size, size_t count, void *arg) {
    struct Block_ *block = (struct Block_ *) arg;
    if (size > block->size_)
        size = block->size_;
    memcpy(buffer, block->data_, size);
    block->data_ += size;
    block->size_ -= size;
    return size;
}

void s3_authorize(
    CURL *curl,
    const char *host,
    uint16_t port,
    const char *method,
    const char *resource,
    const char *id,
    const char *key
) {
    struct curl_slist *headers = NULL;

    char date[] = "Date: Day, ## Mon YEAR HH:MM:SS +0000";
                 /*0123456789012345678901234567890123456*/

    char authorization[] = "Authorization: AWS 01234567890123456789:0123456789012345678901234567";
                          /*01234567890123456789012345678901234567890123456789012345678901234567*/

    time_t now;
    struct tm tm;

    unsigned char digest[20];
    unsigned int result;

    char data[1024];
    size_t size;

    char signature[29];
    char url[1024];

    time(&now);
    localtime_r(&now, &tm);
    strftime(date, sizeof(date), "Date: %a, %d %b %Y %H:%M:%S %z", &tm);

    size = sprintf(data, "%s\n\n\n%s\n%s", method, date + 6, resource);
    result = sizeof(digest);
    HMAC(EVP_sha1(), key, strlen(key), (unsigned char *) data, size, digest, &result);
    base64_encode(signature, digest, sizeof(digest));
    signature[sizeof(signature) - 1] = '\0';

    sprintf(authorization, "Authorization: AWS %s:%s", id, signature);

    headers = curl_slist_append(headers, date);
    headers = curl_slist_append(headers, authorization);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    sprintf(url, "http://%s:%u%s", host, port, resource);
    curl_easy_setopt(curl, CURLOPT_URL, url);
}

static struct option options_[] = {
    {"bits", true, NULL, 'b'},
    {"host", true, NULL, 'h'},
    {"id", true, NULL, 'i'},
    {"key", true, NULL, 'k'},
    {"port", true, NULL, 'p'},
    {"root", true, NULL, 'r'},
    {NULL, false, NULL, 0}
};

static void usage_() {
    fprintf(stderr, "mks3fs --id=<id> --key=<key> [--host=<host>] [--port=<port>] <base>\n");
    exit(1);
}

#define SET_BLOCK(data) do { \
    block.data_ = (const uint8_t *) &(data); \
    block.size_ = sizeof(data); \
    curl_easy_setopt(curl, CURLOPT_INFILE, &block); \
    curl_easy_setopt(curl, CURLOPT_INFILESIZE, sizeof(data)); \
} while (false)

char error_[1024];

void check_(CURLcode code) {
    if (code == 0)
        return;

    fprintf(stderr, "%s: %s\n", argv0_, error_);
    exit(-1);
}

int main(int argc, char *argv[]) {
    CURL *curl;
    int index;

    char uuid[36 + 1];

    const char
        *id = NULL,
        *key = NULL,
        *base = NULL,
        *host = AmazonHost_;

    uint16_t port = AmazonPort_;

    struct ServerSuper_ super;
    struct ServerIndex_ root;

    struct Block_ block;

    char *url;

    struct timeval now;

    memset(&super, 0, sizeof(super));
    memset(&root, 0, sizeof(root));

    argv0_ = argv[0];

    super.bits_ = 14;

    _forever
        switch (getopt_long(argc, argv, "h:i:k:p:r:", options_, &index)) {
            case 'b': {
                char *optend;
                super.bits_ = strtol(optarg, &optend, 10);
                if (optend != optarg + strlen(optarg))
                    usage_();
            } break;

            case 'h':
                host = optarg;
            break;

            case 'i':
                id = optarg;
            break;

            case 'k':
                key = optarg;
            break;

            case 'p': {
                char *optend;
                port = strtol(optarg, &optend, 10);
                if (optend != optarg + strlen(optarg))
                    usage_();
            } break;

            case '?':
                usage_();
            break;

            case -1:
                goto parse;

            default:
                fprintf(stderr, "getopt_long()");
                exit(-1);
            break;
        }

  parse:
    if (
        id == NULL ||
        key == NULL ||
        optind != argc - 1
    )
        usage_();

    base = argv[optind];
    url = (char *) malloc((1 + strlen(base) + 1 + (sizeof(uuid) - 1) + 1) * sizeof(char));

    uuid_generate_time(super.root_);
    uuid_unparse_lower(super.root_, uuid);

    gettimeofday(&now, NULL);

    root.mode_ = S_IFDIR | 0755;
    root.links_ = 2;

    root.accessed_.seconds_ = now.tv_sec;
    root.accessed_.nseconds_ = now.tv_usec * 1000;

    root.modified_.seconds_ = now.tv_sec;
    root.modified_.nseconds_ = now.tv_usec * 1000;

    root.created_.seconds_ = now.tv_sec;
    root.created_.nseconds_ = now.tv_usec * 1000;

    root.user_ = 0;
    root.group_ = 0;

    curl_global_init(CURL_GLOBAL_NOTHING);
    curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_);

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, &on_write_);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, true);

    sprintf(url, "/%s", base);
    s3_authorize(curl, host, port, "PUT", url, id, key);
    SET_BLOCK(super);
    check_(curl_easy_perform(curl));

    sprintf(url, "/%s/%s", base, uuid);
    s3_authorize(curl, host, port, "PUT", url, id, key);
    SET_BLOCK(root);
    check_(curl_easy_perform(curl));

    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return 0;
}
