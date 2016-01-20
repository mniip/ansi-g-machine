#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <execinfo.h>

#include "data.h"

void print_indent(int in)
{
	char *str = malloc(4 * in + 1);
	memset(str, ' ', 4 * in);
	str[4 * in] = 0;
	printf("%s", str);
	free(str);
}

char const *fun_name(void (*fun)())
{
	return backtrace_symbols((void **)&fun, 1)[0];
}

void dump_indent(int in, Closure *c)
{
	size_t i;
	print_indent(in);
#ifdef DUMP_ADDR
	printf("[%p] ", (void *)c);
#endif
	if(c->gc.status & GC_SEEN)
	{
		printf("<seen>\n");
		return;
	}
	c->gc.status |= GC_SEEN;
	switch(c->type)
	{
	case CLOSURE_CON:
		printf("CON");
		if(c->u.con.type == CON_BOX)
		{
			printf("(%d):\n", c->u.con.u.box.variant);
			for(i = 0; c->u.con.u.box.args[i]; i++)
				dump_indent(in + 1, c->u.con.u.box.args[i]);
		}
		else if(c->u.con.type == CON_UNBOX)
		{
			printf("(UNBOX):\n");
			print_indent(in + 1); printf("{");
			for(i = 0; i < c->u.con.u.unbox.size; i++)
			{
				if(i)
					printf(", ");
				printf("%d", ((unsigned char *)c->u.con.u.unbox.data)[i]);
			}
			printf("}\n");
		}
		else
			printf("(?(%d))\n", c->u.con.type);
		break;
	case CLOSURE_THUNK:
		printf("THUNK {%s}/%d:\n", fun_name(c->u.thunk.fun), c->u.thunk.arity);
		for(i = 0; c->u.thunk.args[i]; i++)
			dump_indent(in + 1, c->u.thunk.args[i]);
		break;
	case CLOSURE_BOTTOM:
		printf("BOTTOM:\n");
		dump_indent(in + 1, c->u.bottom);
		break;
	default:
		printf("?(%d)\n", c->type);
		break;
	}
	c->gc.status &= ~GC_SEEN;
}

void dump(Closure *c)
{
	dump_indent(0, c);
}
