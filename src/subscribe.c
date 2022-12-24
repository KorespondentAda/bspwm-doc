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
 * Subscribtions logic
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdarg.h>
#include <fcntl.h>
#include "bspwm.h"
#include "desktop.h"
#include "settings.h"
#include "subscribe.h"
#include "tree.h"

/**
 * Create subscriber list
 *
 * \param stream
 * \param fifo_path
 * \param field
 * \param count
 * \return Created and initialized subscriber list
 */
subscriber_list_t *make_subscriber(FILE *stream, char *fifo_path, int field, int count)
{
	subscriber_list_t *sb = calloc(1, sizeof(subscriber_list_t));
	sb->prev = sb->next = NULL;
	sb->stream = stream;
	sb->fifo_path = fifo_path;
	sb->field = field;
	sb->count = count;
	return sb;
}

/**
 * Remove element from subscriber list
 *
 * \param sb Subscriber to remove
 */
void remove_subscriber(subscriber_list_t *sb)
{
	/** Here is the function workflow: */
	/** Just returns if \p sb is NULL */
	if (sb == NULL) {
		return;
	}

	/** Link neighbour elements */
	subscriber_list_t *a = sb->prev;
	subscriber_list_t *b = sb->next;
	if (a != NULL) {
		a->next = b;
	}
	if (b != NULL) {
		b->prev = a;
	}

	/** Move head to next, tail to previous element */
	if (sb == subscribe_head) {
		subscribe_head = b;
	}
	if (sb == subscribe_tail) {
		subscribe_tail = a;
	}

	/**
	 * If restart is set, remove FD_CLOEXEC flag from stream descriptor.
	 * Else, just close stream and unlink FIFO.
	 */
	if (restart) {
		int cli_fd = fileno(sb->stream);
		fcntl(cli_fd, F_SETFD, ~FD_CLOEXEC & fcntl(cli_fd, F_GETFD));
	} else {
		fclose(sb->stream);
		unlink(sb->fifo_path);
	}
	free(sb->fifo_path);
	free(sb);
}

/**
 * Add new subscriber list element
 * \param sb Subscriber to add
 */
void add_subscriber(subscriber_list_t *sb)
{
	/** Here is the function workflow: */
	/**
	 * If list head isn't set, init head and tail with new element;
	 * Else, append new element at list tail
	 */
	if (subscribe_head == NULL) {
		subscribe_head = subscribe_tail = sb;
	} else {
		subscribe_tail->next = sb;
		sb->prev = subscribe_tail;
		subscribe_tail = sb;
	}

	/** Set FD_CLOEXEC to stream descriptor */
	int cli_fd = fileno(sb->stream);
	fcntl(cli_fd, F_SETFD, FD_CLOEXEC | fcntl(cli_fd, F_GETFD));

	/** If field has SBSC_MASK_REPORT, print_report() then, if need, remove */
	if (sb->field & SBSC_MASK_REPORT) {
		print_report(sb->stream);
		if (sb->count-- == 1) {
			remove_subscriber(sb);
		}
	}
}

/**
 * Print report to given stream
 *
 * \param stream Stream descriptor to print to
 * \return Stream fflush() error code (0 is no error)
 */
int print_report(FILE *stream)
{
	/** Here is the function workflow: */
	/** Print status_prefix */
	fprintf(stream, "%s", status_prefix);
	/** For each monitor, print: */
	for (monitor_t *m = mon_head; m != NULL; m = m->next) {
		/** M — current monitor, m — other; */
		/** Monitor name; */
		fprintf(stream, "%c%s", (mon == m ? 'M' : 'm'), m->name);
		/** For each desktop, */
		for (desktop_t *d = m->desk_head; d != NULL; d = d->next) {
			/** :[ufoUFO] depending on is_urgent() and desktop_t.root, then
			 * desktop name; */
			char c = (is_urgent(d) ? 'u' : (d->root == NULL ? 'f' : 'o'));
			if (m->desk == d) {
				c = toupper(c);
			}
			fprintf(stream, ":%c%s", c, d->name);
		}
		/** If monitor_t.desk isn't NULL (has desktop), */
		if (m->desk != NULL) {
			/** :L[c] depending on used desktop layout, */
			fprintf(stream, ":L%c", LAYOUT_CHR(m->desk->layout));
			if (m->desk->focus != NULL) {
				node_t *n = m->desk->focus;
				/** :T[@c] if focused, depending on window state, */
				if (n->client != NULL) {
					fprintf(stream, ":T%c", STATE_CHR(n->client->state));
				} else {
					fprintf(stream, ":T@");
				}
				int i = 0;
				char flags[5];
				if (n->sticky) {
					flags[i++] = 'S';
				}
				if (n->private) {
					flags[i++] = 'P';
				}
				if (n->locked) {
					flags[i++] = 'L';
				}
				if (n->marked) {
					flags[i++] = 'M';
				}
				flags[i] = '\0';
				/** :G[s] depending on desktop flags; */
				fprintf(stream, ":G%s", flags);
			}
		}
		if (m != mon_tail) {
			/** : separator if not last monitor. */
			fprintf(stream, "%s", ":");
		}
	}
	/** End with newline '\n' then fflush() */
	fprintf(stream, "%s", "\n");
	return fflush(stream);
}

/**
 * Print status message from subscribers of events
 *
 * \param mask Events subscribed for
 * \param ... Message to print for subscribers
 */
void put_status(subscriber_mask_t mask, ...)
{
	/** Here is the function workflow: */
	subscriber_list_t *sb = subscribe_head;
	int ret;
	/**
	 * For each subscriber in list,
	 * if given \p mask presence in subscriber:
	 */
	while (sb != NULL) {
		subscriber_list_t *next = sb->next;
		if (sb->field & mask) {
			/** Decrease subscriber_list_t.count; */
			if (sb->count > 0) {
				sb->count--;
			}
			/**
			 * Print report if SBSC_MASK_REPORT set,
			 * else print formatted by \p ... message;
			 */
			if (mask == SBSC_MASK_REPORT) {
				ret = print_report(sb->stream);
			} else {
				char *fmt;
				va_list args;
				va_start(args, mask);
				fmt = va_arg(args, char *);
				vfprintf(sb->stream, fmt, args);
				va_end(args);
				ret = fflush(sb->stream);
			}
			/** remove_subscriber() if count is zero or something printed. */
			if (ret != 0 || sb->count == 0) {
				remove_subscriber(sb);
			}
		}
		sb = next;
	}
}

/**
 * Remove dead subscribers
 *
 * Remove any subscriber for which the stream has been closed and is no longer
 * writable.
 */
void prune_dead_subscribers(void)
{
	subscriber_list_t *sb = subscribe_head;
	while (sb != NULL) {
		subscriber_list_t *next = sb->next;
		// Is the subscriber's stream closed?
		if (write(fileno(sb->stream), NULL, 0) == -1) {
			remove_subscriber(sb);
		}
		sb = next;
	}
}
