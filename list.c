#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "list.h"

Closure *pack_char(char c)
{
	Closure *ret = new_unbox(sizeof(char));
	*(char *)ret->u.con.u.unbox.data = c;
	return ret;
}

char unpack_char(Closure *c)
{
	if(whnf(NULL, c))
		return 0;
	return *(char *)c->u.con.u.unbox.data;
}

Closure *pack_string(char const *str)
{
	if(*str)
	{
		Closure *ret = new_box(1, 2);
		ret->u.con.u.box.args[0] = pack_char(*str);
		ret->u.con.u.box.args[1] = pack_string(str + 1);
		return ret;
	}
	else
		return new_box(0, 0);
}

char *unpack_string(Closure *c)
{
	if(whnf(NULL, c))
		return NULL;
	switch(c->u.con.u.box.variant)
	{
	case 0:
		{
			char *str = malloc(1);
			str[0] = 0;
			return str;
		}
	case 1:
		{
			Closure *x = c->u.con.u.box.args[0];
			Closure *xs = c->u.con.u.box.args[1];
			gc_enter(x);
			char *old, *str;
			old = unpack_string(xs);
			gc_exit(x);
			if(!old)
				return old;
			str = malloc(strlen(old) + 2);
			str[0] = unpack_char(x);
			str[1] = 0;
			strcat(str, old);
			free(old);
			return str;
		}
	default:
		return NULL;
	}
}

void nil(Closure *self)
{
	replace_box(self, 0, 0);
}

void cons(Closure *self)
{
	Closure *x = self->u.thunk.args[0];
	Closure *xs = self->u.thunk.args[1];
	replace_box(self, 1, 2);
	self->u.con.u.box.args[0] = x;
	self->u.con.u.box.args[1] = xs;
}

void append(Closure *self)
{
	Closure *xw = self->u.thunk.args[0];
	Closure *yw = self->u.thunk.args[1];
	if(whnf(self, xw))
		return;
	switch(xw->u.con.u.box.variant)
	{
	case 0:
		replace(self, yw);
		break;
	case 1:
		{
			Closure *x = xw->u.con.u.box.args[0];
			Closure *xs = xw->u.con.u.box.args[1];
			replace_box(self, 1, 2);
			self->u.con.u.box.args[0] = x;
			self->u.con.u.box.args[1] = apply(apply(invoke(append, 2), xs), yw);
			break;
		}
	}
}

void concat(Closure *self)
{
	Closure *xw = self->u.thunk.args[0];
	if(whnf(self, xw))
		return;
	switch(xw->u.con.u.box.variant)
	{
	case 0:
		replace_box(self, 0, 0);
		break;
	case 1:
		{
			Closure *x = xw->u.con.u.box.args[0];
			Closure *xs = xw->u.con.u.box.args[1];
			replace(self, apply(apply(invoke(append, 2), x), apply(invoke(concat, 1), xs)));
			break;
		}
	}
}
