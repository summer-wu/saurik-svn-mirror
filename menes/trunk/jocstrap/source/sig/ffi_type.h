#ifndef SIG_FFI_TYPE_H
#define SIG_FFI_TYPE_H

#include <apr_pools.h>

#include "sig/types.h"

ffi_type *sig_objc_ffi_type(apr_pool_t *pool, struct sig_type *type);
ffi_type *sig_java_ffi_type(apr_pool_t *pool, struct sig_type *type);

void sig_ffi_types(
    apr_pool_t *pool,
    ffi_type *(*sig_ffi_type)(apr_pool_t *, struct sig_type *),
    struct sig_signature *signature,
    ffi_type **ffi_types,
    size_t skip,
    size_t offset
);

void sig_ffi_cif(
    apr_pool_t *pool,
    ffi_type *(*sig_ffi_type)(apr_pool_t *, struct sig_type *),
    struct sig_signature *signature,
    ffi_cif *ffi_cif,
    ffi_type **types,
    size_t skip,
    size_t offset
);

#endif/*SIG_FFI_TYPE_H*/
