#include "data.h"

Closure *pack_int(int a)
{
	Closure *ret = new_unbox(sizeof(int));
	*(int *)ret->u.con.u.unbox.data = a;
	return ret;
}

int unpack_int(Closure *c)
{
	if(whnf(NULL, c))
		return -1;
	return *(int *)c->u.con.u.unbox.data;
}

void int_add(Closure *self)
{
	int a, b;
	if(whnf(self, self->u.thunk.args[0]))
		return;
	if(whnf(self, self->u.thunk.args[1]))
		return;
	a = *(int *)self->u.thunk.args[0]->u.con.u.unbox.data;
	b = *(int *)self->u.thunk.args[1]->u.con.u.unbox.data;
	replace_unbox(self, sizeof(int));
	*(int *)self->u.con.u.unbox.data = a + b;
}

void int_multiply(Closure *self)
{
	int a, b;
	if(whnf(self, self->u.thunk.args[0]))
		return;
	if(whnf(self, self->u.thunk.args[1]))
		return;
	a = *(int *)self->u.thunk.args[0]->u.con.u.unbox.data;
	b = *(int *)self->u.thunk.args[1]->u.con.u.unbox.data;
	replace_unbox(self, sizeof(int));
	*(int *)self->u.con.u.unbox.data = a * b;
}
