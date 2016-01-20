#ifndef DATA_H_
#define DATA_H_

#include <stddef.h>

#define CLOSURE_CON 0
#define CLOSURE_THUNK 1
#define CLOSURE_BOTTOM 2

#define CON_BOX 0
#define CON_UNBOX 1

#define GC_SEEN 0x1

typedef struct _Closure Closure;
typedef void Thunk(Closure *);

struct _Closure
{
	int type;
	union
	{
		struct
		{
			int type;
			union
			{
				struct
				{
					int variant;
					Closure **args;
				}
				box;
				struct
				{
					size_t size;
					void *data;
				}
				unbox;
			}
			u;
		}
		con;
		struct
		{
			Thunk *fun;
			int arity;
			Closure **args;
		}
		thunk;
		Closure *bottom;
	}
	u;
	struct
	{
		int status;
		int count;
		Closure *next;
	}
	gc;
};

extern Closure *new_closure();
extern Closure *new_box(int, size_t);
extern Closure *new_unbox(size_t);
extern Closure *new_thunk(Thunk *, size_t, int);
extern Closure *new_bottom(Closure *);
extern Closure *invoke(Thunk *, int);
extern void replace_box(Closure *, int, size_t);
extern void replace_unbox(Closure *, size_t);
extern void replace(Closure *, Closure const *);
extern int whnf(Closure *, Closure *);
extern Closure *apply(Closure *, Closure *);
extern void gc_enter(Closure *);
extern void gc_exit(Closure *);
extern void gc();

#endif
