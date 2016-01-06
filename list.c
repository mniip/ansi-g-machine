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
			char *old, *str;
			old = unpack_string(xs);
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

void cons_closure_2(Closure *self)
{
	Closure *x = self->u.thunk.args[0];
	Closure *xs = self->u.thunk.args[1];
	replace_box(self, 1, 2);
	self->u.con.u.box.args[0] = x;
	self->u.con.u.box.args[1] = xs;
}

Closure *cons_closure_1(Closure const *self, Closure *arg)
{
	return curry_thunk(self, cons_closure_2, 2, arg);
}

Closure *cons(Closure const *self, Closure *arg)
{
	return curry_apply(self, cons_closure_1, 1, arg);
}

void append_closure_2(Closure *self)
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
			self->u.con.u.box.args[1] = apply(apply(invoke_function(append), xs), yw);
			break;
		}
	}
}

Closure *append_closure_1(Closure const *self, Closure *arg)
{
	return curry_thunk(self, append_closure_2, 2, arg);
}

Closure *append(Closure const *self, Closure *arg)
{
	return curry_apply(self, append_closure_1, 1, arg);
}
