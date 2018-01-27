/*
 * This file is part of the Poliqarp suite.
 * 
 * Copyright (C) 2004-2009 by Instytut Podstaw Informatyki Polskiej
 * Akademii Nauk (IPI PAN; Institute of Computer Science, Polish
 * Academy of Sciences; cf. www.ipipan.waw.pl).  All rights reserved.
 * 
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file gpl.txt included in the packaging
 * of this file.  (See http://www.gnu.org/licenses/translations.html for
 * unofficial translations.)
 * 
 * A commercial license is available from IPI PAN (contact
 * Michal.Ciesiolka@ipipan.waw.pl or ipi@ipipan.waw.pl for more
 * information).  Licensees holding a valid commercial license from IPI
 * PAN may use this file in accordance with that license.
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
 * THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.
 */

/* 
 * Various OS-dependent functions.
 * Implemented in a file named od_<OSNAME>.c, eg. od_unix.c.
 */

#ifndef POLIQARPD_OSDEP_H
#define POLIQARPD_OSDEP_H

/** 
 * Return the directory (terminated by slash or backslash) containing the 
 * server's configuration file. On some systems, it may return an empty string,
 * indicating that the program's working directory should be used for this 
 * purpose.
 *
 * @return a pointer to a null-terminated string containing the directory 
 * name. 
 */
const char *get_config_dir();

/**
 * Frees memory allocated by a get_config_dir() call (if any).
 */
void free_config_dir();

/** 
 * Sleep for a given number of seconds.
 */
void poliqarp_sleep(int seconds);

/**
 * Intercept all signals that might be necessary, if the platform
 * supports it. 
 */
void handle_signals();


/**
 * Notify a process/thread that poliqarpd is ready to accept connections.
 *
 * Win32: send a WM_APP message to the thread.
 * UNIX: do nothing.
 */
void notify_readiness(unsigned long);


/**
 * Detach from the console.
 */
int console_detach();

/**
 * Produce a message on the standard error output, describing the last error
 * encountered during resolving host names.
 */
void herror(const char *);

/**
 * Produce a message on the standard error output, describing the last socket
 * error.
 */
void print_socket_error(const char *);

#endif /* POLIQARPD_OSDEP_H */
