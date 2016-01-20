#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "data.h"

Closure *gc_head = NULL;

Closure *new_closure()
{
	Closure *ret = malloc(sizeof(Closure));
	ret->gc.status = 0;
	ret->gc.next = gc_head;
	gc_head = ret;
	return ret;
}

Closure *new_box(int variant, size_t len)
{
	Closure *ret = new_closure();
	ret->type = CLOSURE_CON;
	ret->u.con.type = CON_BOX;
	ret->u.con.u.box.variant = variant;
	ret->u.con.u.box.args = calloc(sizeof(Closure *), len + 1);
	ret->u.con.u.box.args[len] = NULL;
	return ret;
}

Closure *new_unbox(size_t size)
{
	Closure *ret = new_closure();
	ret->type = CLOSURE_CON;
	ret->u.con.type = CON_UNBOX;
	ret->u.con.u.unbox.size = size;
	ret->u.con.u.unbox.data = malloc(size);
	return ret;
}

Closure *new_thunk(Thunk *fun, size_t len, int arity)
{
	Closure *ret = new_closure();
	ret->type = CLOSURE_THUNK;
	ret->u.thunk.fun = fun;
	ret->u.thunk.arity = arity;
	ret->u.thunk.args = calloc(sizeof(Closure *), len + 1);
	ret->u.thunk.args[len] = NULL;
	return ret;
}

Closure *invoke(Thunk *fun, int arity)
{
	return new_thunk(fun, 0, arity);
}

Closure *new_bottom(Closure *bottom)
{
	Closure *ret = new_closure();
	ret->type = CLOSURE_BOTTOM;
	ret->u.bottom = bottom;
	return ret;
}

void cleanup_closure(Closure *c)
{
	switch(c->type)
	{
	case CLOSURE_CON:
		if(c->u.con.type == CON_BOX)
			free(c->u.con.u.box.args);
		else if(c->u.con.type == CON_UNBOX)
			free(c->u.con.u.unbox.data);
		break;
	case CLOSURE_THUNK:
		free(c->u.thunk.args);
		break;
	case CLOSURE_BOTTOM:
		break;
	}
}

void replace_box(Closure *self, int variant, size_t len)
{
	cleanup_closure(self);
	self->type = CLOSURE_CON;
	self->u.con.type = CON_BOX;
	self->u.con.u.box.variant = variant;
	self->u.con.u.box.args = calloc(sizeof(Closure *), len + 1);
}

void replace_unbox(Closure *self, size_t size)
{
	cleanup_closure(self);
	self->type = CLOSURE_CON;
	self->u.con.type = CON_UNBOX;
	self->u.con.u.unbox.size = size;
	self->u.con.u.unbox.data = malloc(size);
}

void replace_bottom(Closure *self, Closure *bottom)
{
	cleanup_closure(self);
	self->type = CLOSURE_BOTTOM;
	self->u.bottom = bottom;
}

void replace(Closure *self, Closure const *c)
{
	size_t i;
	if(self == c)
		return;
	cleanup_closure(self);
	self->type = c->type;
	switch(c->type)
	{
	case CLOSURE_CON:
		self->u.con.type = c->u.con.type;
		if(c->u.con.type == CON_BOX)
		{
			self->u.con.u.box.variant = c->u.con.u.box.variant;
			i = 0;
			while(c->u.con.u.box.args[i])
				i++;
			self->u.con.u.box.args = calloc(sizeof(Closure *), i + 1);
			for(i = 0; c->u.con.u.box.args[i]; i++)
				self->u.con.u.box.args[i] = c->u.con.u.box.args[i];
		}
		else if(c->u.con.type == CON_UNBOX)
		{
			self->u.con.u.unbox.size = c->u.con.u.unbox.size;
			self->u.con.u.unbox.data = malloc(c->u.con.u.unbox.size);
			memcpy(self->u.con.u.unbox.data, c->u.con.u.unbox.data, c->u.con.u.unbox.size);
		}
		break;
	case CLOSURE_THUNK:
		self->u.thunk.fun = c->u.thunk.fun;
		self->u.thunk.arity = c->u.thunk.arity;
		i = 0;
		while(c->u.thunk.args[i])
			i++;
		self->u.thunk.args = calloc(sizeof(Closure *), i + 1);
		for(i = 0; c->u.thunk.args[i]; i++)
			self->u.thunk.args[i] = c->u.thunk.args[i];
		break;
	case CLOSURE_BOTTOM:
		self->u.bottom = c->u.bottom;
		break;
	}
}

int whnf(Closure *self, Closure *c)
{
	while(c->type == CLOSURE_THUNK && !c->u.thunk.arity)
	{
		c->u.thunk.fun(c);
		if(self)
			gc_enter(self);
		gc_enter(c);
		gc();
		gc_exit(c);
		if(self)
			gc_exit(self);
	}
	if(c->type == CLOSURE_BOTTOM)
	{
		if(self)
			replace_bottom(self, c->u.bottom);
		return 1;
	}
	return 0;
}

Closure *apply(Closure *fun, Closure *arg)
{
	size_t i;
	Closure *ret;
	if(whnf(NULL, fun))
		return fun;
	i = 0;
	while(fun->u.thunk.args[i])
		i++;
	ret = new_thunk(fun->u.thunk.fun, i + 1, fun->u.thunk.arity - 1);
	ret->u.thunk.args[i] = arg;
	for(i = 0; fun->u.thunk.args[i]; i++)
		ret->u.thunk.args[i] = fun->u.thunk.args[i];
	return ret;
}

void gc_enter(Closure *c)
{
	c->gc.count++;
}

void gc_exit(Closure *c)
{
	c->gc.count--;
}

void gc_mark(Closure *c)
{
	size_t i;
	if(c->gc.status & GC_SEEN)
		return;
	c->gc.status |= GC_SEEN;
	switch(c->type)
	{
	case CLOSURE_CON:
		if(c->u.con.type == CON_BOX)
			for(i = 0; c->u.con.u.box.args[i]; i++)
				gc_mark(c->u.con.u.box.args[i]);
		break;
	case CLOSURE_THUNK:
		for(i = 0; c->u.thunk.args[i]; i++)
			gc_mark(c->u.thunk.args[i]);
		break;
	case CLOSURE_BOTTOM:
		gc_mark(c->u.bottom);
		break;
	}
}

void destroy_closure(Closure *c)
{
	cleanup_closure(c);
	free(c);
}

void gc_collect()
{
	Closure **ptr;
	Closure *cur = gc_head;
	while(cur)
	{
		if(cur->gc.count)
			gc_mark(cur);
		cur = cur->gc.next;
	}
	ptr = &gc_head;
	while(*ptr)
	{
		if(!((*ptr)->gc.status & GC_SEEN))
		{
			Closure *next = (*ptr)->gc.next;
			destroy_closure(*ptr);
			*ptr = next;
			continue;
		}
		(*ptr)->gc.status &= ~GC_SEEN;
		ptr = &(*ptr)->gc.next;
	}
}

void gc()
{
	gc_collect();
}
