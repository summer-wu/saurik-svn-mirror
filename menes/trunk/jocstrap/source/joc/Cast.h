#ifndef JOC_CASTS_H
#define JOC_CASTS_H

#include <apr_pools.h>

#include <jni.h>

#include "sig/types.h"

void java_cast_from(
    apr_pool_t *pool,
    struct sig_type *sig_type,
    ffi_type *ffi_type,
    uint8_t *value,
    JNIEnv *env,
    jobject object
);

jobject java_cast_to(
    apr_pool_t *pool,
    struct sig_type *sig_type,
    ffi_type *ffi_type,
    uint8_t *value,
    JNIEnv *env
);

void java_copy_from(
    apr_pool_t *pool,
    struct sig_type *sig_type,
    ffi_type *ffi_type,
    uint8_t *value,
    JNIEnv *env,
    uint8_t *result
);

void java_copy_to(
    apr_pool_t *pool,
    struct sig_type *sig_type,
    ffi_type *ffi_type,
    uint8_t *value,
    JNIEnv *env,
    uint8_t *result
);

#endif/*JOC_CASTS_H*/
