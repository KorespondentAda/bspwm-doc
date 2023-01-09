/* Copyright (c) 2012, Bastien Dejean
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file
 * \todo Describe functions
 */

#include <stdlib.h>
#include "bspwm.h"
#include "window.h"
#include "subscribe.h"
#include "ewmh.h"
#include "tree.h"
#include "stack.h"

/** [Stack](#stacking_list_t) constructor.
 *
 * Create stack item from specified [node](#node_t).
 * Returned item can be inserted by
 * stack_insert_after() or stack_insert_before().
 *
 * \param n Node to construct stack item from
 * \return Constructed stack
 */
stacking_list_t *make_stack(node_t *n)
{
	stacking_list_t *s = calloc(1, sizeof(stacking_list_t));
	s->node = n;
	s->prev = s->next = NULL;
	return s;
}

/** Insert node in stack after specified item
 *
 * Create stack item from node \p n and insert it right after \p a.
 * If \p a is NULL, #stack_head and #stack_tail sets to new stack item.
 *
 * \param a Stack item to insert before
 * \param n Node to insert
 * \warn If \p a is NULL when stack already exists, it will be missed.
 */
void stack_insert_after(stacking_list_t *a, node_t *n)
{
	stacking_list_t *s = make_stack(n);
	if (a == NULL) {
		stack_head = stack_tail = s;
	} else {
		if (a->node == n) {
			free(s);
			return;
		}
		remove_stack_node(n);
		stacking_list_t *b = a->next;
		if (b != NULL) {
			b->prev = s;
		}
		s->next = b;
		s->prev = a;
		a->next = s;
		if (stack_tail == a) {
			stack_tail = s;
		}
	}
}

/** Insert node in stack before specified item
 *
 * Create stack item from node \p n and insert it in front of \p a.
 * If \p a is NULL, #stack_head and #stack_tail sets to new stack item.
 *
 * \param a Stack item to insert before
 * \param n Node to insert
 * \warn If \p a is NULL when stack already exists, it will be missed.
 */
void stack_insert_before(stacking_list_t *a, node_t *n)
{
	stacking_list_t *s = make_stack(n);
	if (a == NULL) {
		stack_head = stack_tail = s;
	} else {
		if (a->node == n) {
			free(s);
			return;
		}
		remove_stack_node(n);
		stacking_list_t *b = a->prev;
		if (b != NULL) {
			b->next = s;
		}
		s->prev = b;
		s->next = a;
		a->prev = s;
		if (stack_head == a) {
			stack_head = s;
		}
	}
}

/** [Stack](#stacking_list_t) destructor.
 *
 * Remove specified item from stack, saving stack structure and freeing memory.
 *
 * \param s Stack item
 */
void remove_stack(stacking_list_t *s)
{
	if (s == NULL) {
		return;
	}
	stacking_list_t *a = s->prev;
	stacking_list_t *b = s->next;
	if (a != NULL) {
		a->next = b;
	}
	if (b != NULL) {
		b->prev = a;
	}
	if (s == stack_head) {
		stack_head = b;
	}
	if (s == stack_tail) {
		stack_tail = a;
	}
	free(s);
}

/** Remove all items with node
 *
 * Removes all items from [stack](#stacking_list_t) that contains specified node
 * or its child.
 *
 * \param n Node to remove from stack
 */
void remove_stack_node(node_t *n)
{
	/** \todo What is \ref first_extrema() and \ref next_leaf() */
	for (node_t *f = first_extrema(n); f != NULL; f = next_leaf(f, n)) {
		for (stacking_list_t *s = stack_head; s != NULL; s = s->next) {
			if (s->node == f) {
				remove_stack(s);
				break;
			}
		}
	}
}

/** Client stack depth level
 *
 * Level is $3\cdot a + b$, where:
 * - $a$ is layer level {LAYER_BELOW, LAYER_NORMAL, LAYER_??}
 * - $b$ is state level {IS_TILED(c), IS_FLOATING(c), IS_??}
 *
 * \todo Insert layer and state value macros
 *
 * \param c Client node
 * \return Stack level of given client
 */
int stack_level(client_t *c)
{
	int layer_level = (c->layer == LAYER_NORMAL ? 1 : (c->layer == LAYER_BELOW ? 0 : 2));
	int state_level = (IS_TILED(c) ? 0 : (IS_FLOATING(c) ? 1 : 2));
	return 3 * layer_level + state_level;
}

/** Stack comparsion
 *
 * Compare by stack_level()
 * \param c1 First client
 * \param c2 Second client
 * \return Difference between compared values
 */
int stack_cmp(client_t *c1, client_t *c2)
{
	return stack_level(c1) - stack_level(c2);
}

/** Get last stack item above specified node
 *
 * See stack from head to get last item that above given node or on equal level.
 *
 * \param n Node to filter
 * \return Last stack item that is above than \p n node
 */
stacking_list_t *limit_above(node_t *n)
{
	stacking_list_t *s = stack_head;
	while (s != NULL && stack_cmp(n->client, s->node->client) >= 0) {
		s = s->next;
	}
	if (s == NULL) {
		s = stack_tail;
	}
	if (s->node == n) {
		s = s->prev;
	}
	return s;
}

/** Get last stack item below specified node
 *
 * See stack from tail to get last item that below given node or on equal level.
 *
 * \param n Node to filter
 * \return Last stack item that is above than \p n node
 */
stacking_list_t *limit_below(node_t *n)
{
	stacking_list_t *s = stack_tail;
	while (s != NULL && stack_cmp(n->client, s->node->client) <= 0) {
		s = s->prev;
	}
	if (s == NULL) {
		s = stack_head;
	}
	if (s->node == n) {
		s = s->next;
	}
	return s;
}

/** \todo Describe
 */
void stack(desktop_t *d, node_t *n, bool focused)
{
	/** Here is the function workflow: */
	/** For every leaf of \p n (next_leaf()): */
	for (node_t *f = first_extrema(n); f != NULL; f = next_leaf(f, n)) {
		/** Skip NULL clients, and floating clients if #auto_raise isn't set; */
		if (f->client == NULL || (IS_FLOATING(f->client) && !auto_raise)) {
			continue;
		}

		if (stack_head == NULL) {
			stack_insert_after(NULL, f);
		} else {
			stacking_list_t *s = (focused ? limit_above(f) : limit_below(f));
			if (s == NULL) {
				continue;
			}
			int i = stack_cmp(f->client, s->node->client);
			if (i < 0 || (i == 0 && !focused)) {
				stack_insert_before(s, f);
				window_below(f->id, s->node->id);
				put_status(SBSC_MASK_NODE_STACK, "node_stack 0x%08X below 0x%08X\n", f->id, s->node->id);
			} else {
				stack_insert_after(s, f);
				window_above(f->id, s->node->id);
				put_status(SBSC_MASK_NODE_STACK, "node_stack 0x%08X above 0x%08X\n", f->id, s->node->id);
			}
		}
	}

	ewmh_update_client_list(true);
	restack_presel_feedbacks(d);
}

/** \todo Describe
 */
void restack_presel_feedbacks(desktop_t *d)
{
	stacking_list_t *s = stack_tail;
	while (s != NULL && !IS_TILED(s->node->client)) {
		s = s->prev;
	}
	if (s != NULL) {
		restack_presel_feedbacks_in(d->root, s->node);
	}
}

/** \todo Describe
 */
void restack_presel_feedbacks_in(node_t *r, node_t *n)
{
	if (r == NULL) {
		return;
	} else {
		if (r->presel != NULL) {
			window_above(r->presel->feedback, n->id);
		}
		restack_presel_feedbacks_in(r->first_child, n);
		restack_presel_feedbacks_in(r->second_child, n);
	}
}
