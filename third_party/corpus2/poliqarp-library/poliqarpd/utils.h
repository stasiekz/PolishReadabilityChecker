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

/** Useful functions and definitions. */

#ifndef POLIQARPD_UTILS_H
#define POLIQARPD_UTILS_H

#ifdef MIN
# undef MIN
#endif
#define MIN(a, b)   (((a) > (b)) ? (b) : (a))

#ifdef MAX
# undef MAX
#endif
#define MAX(a, b)   (((a) > (b)) ? (a) : (b))

/** These two macros are defined for easy locking and unlocking of mutexes. */
#define lock(m)                          \
   pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock, (void *)(m)); \
   do {                                  \
      if (pthread_mutex_lock(m))         \
         system_error("mutex lock failed"); \
   } while (0)

#define unlock(m) pthread_cleanup_pop(1)      

/** 
 * Prints out the given error message (along with the system error message) 
 * on stderr and terminates the process. 
 */
void system_error(const char *msg);

/** 
 * Prints out the given error message (along with the socket error message) 
 * on stderr and terminates the process. 
 */
void socket_error(const char *msg);

/** 
 * Prints out the given error message (along with the resolver error message) 
 * on stderr and terminates the process. 
 */
void resolve_error(const char *msg);

#endif /* POLIQARPD_UTILS_H */
