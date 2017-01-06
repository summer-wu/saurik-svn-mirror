#ifndef SIG_PARSE_H
#define SIG_PARSE_H

#include "sig/types.h"

#include <apr_pools.h>

extern void (*sig_aggregate)(apr_pool_t *pool, enum joc_primitive primitive, const char *name, struct sig_signature *signature, const char *types);

void sig_parse_signature(apr_pool_t *pool, struct sig_signature *signature, const char **name, char eos);
struct sig_type *sig_parse_type(apr_pool_t *pool, const char **name, char eos, bool named);
void sig_parse(apr_pool_t *pool, struct sig_signature *signature, const char *name);

const char *sig_unparse_signature(apr_pool_t *pool, struct sig_signature *signature);
const char *sig_unparse_type(apr_pool_t *pool, struct sig_type *type);

#endif/*SIG_PARSE_H*/
