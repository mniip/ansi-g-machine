#ifndef INT_H_
#define INT_H_

#include "data.h"

extern Closure *pack_int(int);
extern int unpack_int(Closure *);

extern Function int_add;
extern Function int_multiply;

#endif
