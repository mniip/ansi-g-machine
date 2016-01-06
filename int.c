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

void int_add_closure_2(Closure *self)
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

Closure *int_add_closure_1(Closure const *self, Closure *arg)
{
	return curry_thunk(self, int_add_closure_2, 2, arg);
}

Closure *int_add(Closure const *self, Closure *arg)
{
	return curry_apply(self, int_add_closure_1, 1, arg);
}

void int_multiply_closure_2(Closure *self)
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

Closure *int_multiply_closure_1(Closure const *self, Closure *arg)
{
	return curry_thunk(self, int_multiply_closure_2, 2, arg);
}

Closure *int_multiply(Closure const *self, Closure *arg)
{
	return curry_apply(self, int_multiply_closure_1, 1, arg);
}
