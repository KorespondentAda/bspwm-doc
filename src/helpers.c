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
 * Number of useful functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include "bspwm.h"

/** Warning message
 *
 * \param fmt Format string (printf-like)
 * \param ... Format args
 *
 * Prints warning specified by \p fmt to `stderr`.
 */
void warn(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}

__attribute__((noreturn))
/** Error occurence
 *
 * \param fmt Format string (printf-like)
 * \param ... Format args
 *
 * Prints error message specified by \p fmt to `stderr`
 * and terminate execution.
 */
void err(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

/** Read file to string
 *
 * \param [in] file_path Path to file to read
 * \param [out] tlen Length of readed data
 * \return Pointer to readed string, or NULL if error occurs
 *
 * \todo Check if it faster to remove `goto`'s
 */
char *read_string(const char *file_path, size_t *tlen)
{
	if (file_path == NULL) {
		return NULL;
	}

	int fd = open(file_path, O_RDONLY);

	if (fd == -1) {
		perror("Read file: open");
		return NULL;
	}

	/** Uses BUFSIZ as read buffer size */
	char buf[BUFSIZ], *content = NULL;
	size_t len = sizeof(buf);

	if ((content = calloc(len, sizeof(char))) == NULL) {
		perror("Read file: calloc");
		goto end;
	}

	int nb;
	*tlen = 0;

	while (true) {
		nb = read(fd, buf, sizeof(buf));
		if (nb < 0) {
			perror("Restore tree: read");
			free(content);
			content = NULL;
			goto end;
		} else if (nb == 0) {
			break;
		} else {
			*tlen += nb;
			if (*tlen > len) {
				len *= 2;
				char *rcontent = realloc(content, len * sizeof(char));
				if (rcontent == NULL) {
					perror("Read file: realloc");
					free(content);
					content = NULL;
					goto end;
				} else {
					content = rcontent;
				}
			}
			strncpy(content + (*tlen - nb), buf, nb);
		}
	}

end:
	close(fd);
	return content;
}

/** Get string copy
 *
 * \param str String to copy
 * \param len Length of given string
 * \return Pointer to copy of given string; NULL if error occured
 */
char *copy_string(char *str, size_t len)
{
	/** \todo Why there used that calloc() form? */
	char *cpy = calloc(1, ((len+1) * sizeof(char)));
	if (cpy == NULL) {
		perror("Copy string: calloc");
		return NULL;
	}
	strncpy(cpy, str, len);
	cpy[len] = '\0'; /** Always end copied string with \0 */
	return cpy;
}

/** Create temporary pipe FIFO
 *
 * \param template Filename template to use
 * \return Path to created temporary pipe FIFO
 *
 * \sa FIFO_TEMPLATE, cmd_subscribe()
 * \warning Segmentation fault if template is NULL
 * \todo Is it rentable to create, open, close and unlink file to get temporary
 * name?
 */
char *mktempfifo(const char *template)
{
	int tempfd;
	/** Get current runtime dir from env, or '`/tmp`' */
	char *runtime_dir = getenv(RUNTIME_DIR_ENV);
	if (runtime_dir == NULL) {
		runtime_dir = "/tmp";
	}

	char *fifo_path = malloc(strlen(runtime_dir)+1+strlen(template)+1);
	if (fifo_path == NULL) {
		return NULL;
	}

	/**
	 * Set \p fifo_path as `runtime_dir/template`
	 */
	sprintf(fifo_path, "%s/%s", runtime_dir, template);

	/** Generate unique filename and open it by mkstemp() */
	if ((tempfd = mkstemp(fifo_path)) == -1) {
		free(fifo_path);
		return NULL;
	}

	/** Then close() and unlink() it */
	close(tempfd);
	unlink(fifo_path);

	/** Create \p fifo_path pipe fifo with 0666 permissions */
	if (mkfifo(fifo_path, 0666) == -1) {
		free(fifo_path);
		return NULL;
	}

	return fifo_path;
}

/** Print formatted string to buffer string
 *
 * \param [out] buf Buffer output buffer
 * \param [in] fmt Format string (printf-like)
 * \param [in] ... Format string arguments
 * \return Size of printed data, -1 if error occurs
 *
 * \note Realization of GNU extension function
 *
 * Allocates enough memory on \p buf, then print to like sprintf()
 */
int asprintf(char **buf, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int size = vasprintf(buf, fmt, args);
	va_end(args);
	return size;
}

/** Print formatted string to buffer string
 *
 * \param [out] buf Buffer output buffer
 * \param [in] fmt Format string (printf-like)
 * \param [in] args List of arguments to \p fmt
 * \return Size of printed data, -1 if error occurs
 *
 * \note Realization of GNU extension function
 *
 * Allocates enough memory on \p buf, then print to like sprintf()
 */
int vasprintf(char **buf, const char *fmt, va_list args)
{
	va_list tmp;
	va_copy(tmp, args);
	int size = vsnprintf(NULL, 0, fmt, tmp);
	va_end(tmp);

	if (size < 0) {
		return -1;
	}

	*buf = malloc(size + 1);

	if (*buf == NULL) {
		return -1;
	}

	size = vsprintf(*buf, fmt, args);
	return size;
}

/** Check string to be color specifier like "#RRGGBB"
 *
 * \param color String to check
 * \return True if conditions met, false otherwise
 *
 * Conditions:
 * - String length is 7 by strlen() (8 with terminator)
 * - String starts with '#'
 * - Next 6 chars is hexadecimal digits by isxdigit()
 *
 * \warning Segmentation fault if \p color is NULL
 */
bool is_hex_color(const char *color)
{
	if (color[0] != '#' || strlen(color) != 7) {
		return false;
	}
	for (int i = 1; i < 7; i++) {
		if (!isxdigit(color[i])) {
			return false;
		}
	}
	return true;
}

/** Tokenize string by separator with escape characters
 *
 * \param state New or current tokenization state
 * \param s String to tokenize, NULL to get next token
 * \param sep Token separator
 * \return Next token string; NULL if string processed or error occured
 *
 * First call sets string \p s to process,
 * then returns next token separated by \p sep.
 */
char *tokenize_with_escape(struct tokenize_state *state, const char *s, char sep)
{
	/** Here is the function workflow: */
	/** On first call (\p s is not NULL) initializes \p state structure */
	if (s != NULL) {
		state->in_escape = false;
		state->pos = s;
		state->len = strlen(s) + 1;
	}

	char *outp = calloc(state->len, 1); /** Allocate state.len bytes */
	char *ret = outp; /** Save start of buffer to return it */
	if (!outp) return NULL;

	/** Iterate by state.pos */
	char cur;
	while (*state->pos) {
		--state->len;
		cur = *state->pos++;

		/** Unescape (`\x`) characters */
		if (state->in_escape) {
			*outp++ = cur;
			state->in_escape = false;
			continue;
		}

		if (cur == '\\') {
			state->in_escape = !state->in_escape;
		} else if (cur == sep) {
			return ret;
		} else {
			*outp++ = cur;
		}
	}

	return ret;
}
