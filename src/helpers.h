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

#ifndef BSPWM_HELPERS_H
#define BSPWM_HELPERS_H

/** \file helpers.h
 * Useful helper functions and macros
 * \todo Add doxygen groups for macros
 */

#include <xcb/xcb.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <float.h>

/** Length of non-byte array */
#define LENGTH(x)         (sizeof(x) / sizeof(*x))
/** Maximum of two values */
#define MAX(A, B)         ((A) > (B) ? (A) : (B))
/** Minimum of two values */
#define MIN(A, B)         ((A) < (B) ? (A) : (B))

/** Is node state is [tiled](#STATE_TILED)
 * or [pseudo tiled](#STATE_PSEUDO_TILED)
 */
#define IS_TILED(c)       (c->state == STATE_TILED || c->state == STATE_PSEUDO_TILED)
/** Is node state is [floating](#STATE_FLOATING) */
#define IS_FLOATING(c)    (c->state == STATE_FLOATING)
/** Is node state is [fullscreen](#STATE_FULLSCREEN) */
#define IS_FULLSCREEN(c)  (c->state == STATE_FULLSCREEN)
/** Is node state is [tiled](#STATE_TILED) */
#define IS_RECEPTACLE(n)  (is_leaf(n) && n->client == NULL)

/* Convertions to string */
/** Convert condition to "true" or "false" string */
#define BOOL_STR(A)       ((A) ? "true" : "false")
/** Convert condition to "on" or "off" string */
#define ON_OFF_STR(A)     ((A) ? "on" : "off")
/** Convert [layout type](#layout_t) member to "tiled" or "monocle" string */
#define LAYOUT_STR(A)     ((A) == LAYOUT_TILED ? "tiled" : "monocle")
/** Convert [layout type](#layout_t) to 'T' or 'M' char */
#define LAYOUT_CHR(A)     ((A) == LAYOUT_TILED ? 'T' : 'M')
/** Convert [child polarity](#child_polarity_t) to string */
#define CHILD_POL_STR(A)  ((A) == FIRST_CHILD ? "first_child" : "second_child")
/** Convert [split autoscheme](#automatic_scheme_t) to string */
#define AUTO_SCM_STR(A)   ((A) == SCHEME_LONGEST_SIDE ? "longest_side" : ((A) == SCHEME_ALTERNATE ? "alternate" : "spiral"))
/** Convert #tightness_t to string */
#define TIGHTNESS_STR(A)  ((A) == TIGHTNESS_HIGH ? "high" : "low")
/** Convert [Split type](#split_type_t) to string */
#define SPLIT_TYPE_STR(A) ((A) == TYPE_HORIZONTAL ? "horizontal" : "vertical")
/** Convert [Split mode](#split_mode_t) to string */
#define SPLIT_MODE_STR(A) ((A) == MODE_AUTOMATIC ? "automatic" : "manual")
/** Convert [direction specifier](#direction_t) to string */
#define SPLIT_DIR_STR(A)  ((A) == DIR_NORTH ? "north" : ((A) == DIR_WEST ? "west" : ((A) == DIR_SOUTH ? "south" : "east")))
/** Convert [node state](#client_state_t) to string */
#define STATE_STR(A)      ((A) == STATE_TILED ? "tiled" : ((A) == STATE_FLOATING ? "floating" : ((A) == STATE_FULLSCREEN ? "fullscreen" : "pseudo_tiled")))
/** Convert [node state](#client_state_t) to char */
#define STATE_CHR(A)      ((A) == STATE_TILED ? 'T' : ((A) == STATE_FLOATING ? 'F' : ((A) == STATE_FULLSCREEN ? '=' : 'P')))
/** Convert #stack_layer_t to string */
#define LAYER_STR(A)      ((A) == LAYER_BELOW ? "below" : ((A) == LAYER_NORMAL ? "normal" : "above"))

/** Imploding of XCB_CONFIG_WINDOW_X and XCB_CONFIG_WINDOW_Y macros */
#define XCB_CONFIG_WINDOW_X_Y               (XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y)
/** Imploding of XCB_CONFIG_WINDOW_WIDTH and XCB_CONFIG_WINDOW_HEIGHT macros */
#define XCB_CONFIG_WINDOW_WIDTH_HEIGHT      (XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT)
/** Imploding of XCB_CONFIG_WINDOW_X_Y and XCB_CONFIG_WINDOW_WIDTH_HEIGHT macros */
#define XCB_CONFIG_WINDOW_X_Y_WIDTH_HEIGHT  (XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT)

/** Maximum string length
 * Used only for const string definition and haven't checked
 */
#define MAXLEN    256
/** Size for small-length string arrays */
#define SMALEN     32
/** Initial capacity of message arguments array \todo Move to messages.c? */
#define INIT_CAP    8

/** Clean modfield from #num_lock, #scroll_lock and #caps_lock mod bits */
#define cleaned_mask(m)   (m & ~(num_lock | scroll_lock | caps_lock))
/** Strings comparsion for equality */
#define streq(s1, s2)     (strcmp((s1), (s2)) == 0)
/** Substraction without overloading
 * \todo Maybe rewrite as ternary:
 * `(a -= MIN(a, b))`
 */
#define unsigned_subtract(a, b)  \
	do {                         \
		if (b > a) {             \
			a = 0;               \
		} else {                 \
			a -= b;              \
		}                        \
	} while (false)


void warn(char *fmt, ...);
void err(char *fmt, ...);
char *read_string(const char *file_path, size_t *tlen);
char *copy_string(char *str, size_t len);
char *mktempfifo(const char *template);
int asprintf(char **buf, const char *fmt, ...);
int vasprintf(char **buf, const char *fmt, va_list args);
bool is_hex_color(const char *color);

/**
 * State for continuous tokenization
 */
struct tokenize_state {
	bool in_escape;    /**< Is currently processed symbol must be escaped */
	const char *pos;   /**< Pointer to remaining string */
	size_t len;        /**< Length of remaining string */
};
char *tokenize_with_escape(struct tokenize_state *state, const char *s, char sep);

#endif
