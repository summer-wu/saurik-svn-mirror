#include "joc/Memory.h"

apr_pool_t *apr_pool_ = NULL;

void mem_Initialize() {
    apr_initialize();
    apr_pool_create(&apr_pool_, NULL);
}
