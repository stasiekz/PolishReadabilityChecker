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

/**
 * Declaration of a structure holding poliqarpd's configuration and
 * functions used to handle it.
 */

#ifndef POLIQARPD_CONFIGURATION_H
#define POLIQARPD_CONFIGURATION_H

#include <poliqarp-config.h>

#include <foostring/foostring.h>
#include <pthread.h>
#include <stdio.h>


struct configuration {
   /* Static configuration */
 
   string_t hostname;        /**< the host that we're listening on */
   uint16_t port;            /**< the port that we're listening on */
   int logging_on;           /**< logging enabled? */
   string_t logfile;         /**< name of the file that we're logging to */
   size_t match_buffer_size; /**< initial size of match buffer */
   time_t max_session_idle;  /**< how long a session can be idle */
   size_t max_match_length;  /**< maximum match length, in segments */
   struct hash_table* corpora;   /**< collection of predefined corpora */
   bool allow_any_corpus;        /**< allow to open any corpus in the file system */

   /* Dynamic configuration */

   bool gui_mode;            /**< GUI mode: whether to terminate the server
                                  when the number of active sessions drops to
                                  zero */
   bool detach;              /**< deamon mode */
   unsigned long notify_thread_id;
                             /**< thread to be notified when the server is
                                  ready to accept connections (Win32 only) */
   FILE *log;                /**< physical counterpart of logfile */
   pthread_mutex_t logmutex; /**< mutex that synchronizes log calls to 
                                  localtime() */
};

/** The global configuration object. */
extern struct configuration cfg;

/** Initializes the configuration object. */
void init_default_cfg();

/** Frees all resources associated with a configuration. */
void done_cfg();

/** Reads the configuration from a given file and store it in the memory area 
 * pointed to by cfg. Outputs any warning messages (about wrong options, etc.)
 * to stderr.
 * @return 0 if the configuration was read successfully (possibly with 
 * warnings).
 * @return -1 if an I/O error occurred.
 */
int read_cfg(const char *filename);

#endif /* POLIQARPD_CONFIGURATION_H */
