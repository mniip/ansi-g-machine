#ifndef LIST_H_
#define LIST_H_

extern Closure *pack_char(char);
extern char unpack_char(Closure *);

extern Closure *pack_string(char const *);
extern char *unpack_string(Closure *);

extern Thunk nil;
extern Thunk cons;

extern Thunk append;
extern Thunk concat;

#endif
