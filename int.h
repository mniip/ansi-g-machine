#ifndef INT_H_
#define INT_H_

#include "data.h"

extern Closure *pack_int(int);
extern int unpack_int(Closure *);

extern Thunk int_add;
extern Thunk int_multiply;

#endif
