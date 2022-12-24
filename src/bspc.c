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
 * Main file of BSPC instance
 *
 * Command `bspc` used to send messages to running BSPWM instance
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <sys/un.h>
#include <unistd.h>
#include "helpers.h"
#include "common.h"

/**
 * Main function of BSPC instance
 *
 * Command line arguments:
 * - `--print-socket-path` — print gotten socket path and exit;
 * - Other — parts of message to send.
 *
 * Here is the function workflow:
 */
int main(int argc, char *argv[])
{
	int sock_fd;
	struct sockaddr_un sock_address;
	/** Using BUFSIZ as size for message and response buffers. */
	char msg[BUFSIZ], rsp[BUFSIZ];

	/**
	 * Check for at least one command line argument presence.
	 */
	if (argc < 2) {
		err("No arguments given.\n");
	}

	/**
	 * Using UNIX socket for communication with BSPWM.
	 * Socket path got from SOCKET_ENV_VAR.
	 */
	sock_address.sun_family = AF_UNIX;
	char *sp;

	sp = getenv(SOCKET_ENV_VAR);
	if (sp != NULL) {
		/** If present, save it as socket path; */
		snprintf(sock_address.sun_path, sizeof(sock_address.sun_path), "%s", sp);
	} else {
		/** Else, form socket path by [SOCKET_PATH_TPL](#SOCKET_PATH_TPL) with data from
		 * xcb_parse_display() call, based on DISPLAY envvar.
		 */
		char *host = NULL;
		int dn = 0, sn = 0;
		if (xcb_parse_display(NULL, &host, &dn, &sn) != 0) {
			snprintf(sock_address.sun_path, sizeof(sock_address.sun_path), SOCKET_PATH_TPL, host, dn, sn);
		}
		free(host);
	}

	/**
	 * Option `--print-socket-path` — print gotten socket path and exit.
	 */
	if (streq(argv[1], "--print-socket-path")) {
		printf("%s\n", sock_address.sun_path);
		return EXIT_SUCCESS;
	}

	/** Open socket with gotten path, exit with err() on failure. */
	if ((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		err("Failed to create the socket.\n");
	}

	/** Connect to socket, exit with err() on failure. */
	if (connect(sock_fd, (struct sockaddr *) &sock_address, sizeof(sock_address)) == -1) {
		err("Failed to connect to the socket.\n");
	}

	argc--, argv++;
	int msg_len = 0;

	/** Form message to send by \0-separated command line arguments */
	for (int offset = 0, rem = sizeof(msg), n = 0;
			argc > 0 && rem > 0;
			offset += n, rem -= n, argc--, argv++) {
		n = snprintf(msg + offset, rem, "%s%c", *argv, 0);
		msg_len += n;
	}

	/** Send message to BSPWM, exit with err() on failure. */
	if (send(sock_fd, msg, msg_len, 0) == -1) {
		err("Failed to send the data.\n");
	}

	int ret = EXIT_SUCCESS, nb;

	/**
	 * Prepare to poll():
	 * - From socket wait for POLLIN for response from BSPWM to print;
	 * - From FIFO wait for POLLHUP to exit when stdout disconnected.
	 */
	struct pollfd fds[] = {
		{sock_fd, POLLIN, 0},
		{STDOUT_FILENO, POLLHUP, 0},
	};

	while (poll(fds, 2, -1) > 0) {
		if (fds[0].revents & POLLIN) {
			if ((nb = recv(sock_fd, rsp, sizeof(rsp)-1, 0)) > 0) {
				rsp[nb] = '\0';
				if (rsp[0] == FAILURE_MESSAGE[0]) {
					ret = EXIT_FAILURE;
					fprintf(stderr, "%s", rsp + 1);
					fflush(stderr);
				} else {
					fprintf(stdout, "%s", rsp);
					fflush(stdout);
				}
			} else {
				break;
			}
		}
		if (fds[1].revents & (POLLERR | POLLHUP)) {
			break;
		}
	}

	close(sock_fd);
	return ret;
}
