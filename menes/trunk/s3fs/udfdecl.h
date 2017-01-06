#ifndef UDFDECL_H_
#define UDFDECL_H_

#include <linux/time.h>
#include "ecma_167.h"

kernel_timestamp *udf_time_to_stamp(kernel_timestamp *dest, struct timespec ts);

#endif/*UDFDECL_H_*/
