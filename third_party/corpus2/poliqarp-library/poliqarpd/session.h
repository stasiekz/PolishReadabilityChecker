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

#ifndef POLIQARPD_SESSION_H
#define POLIQARPD_SESSION_H

#include "poliqarp-config.h"

#ifdef HAVE_LOCALE_T
#include <locale.h>
#endif

#include <foostring/foostring.h>
#include <sakura/poliqarp.h>
#include <sakura/random.h>
#include <pthread.h>
#include <time.h>

#include "configuration.h"
#include "msgqueue.h"
#include "server.h"
#include "sessopt.h"

#define MAX_SESSIONS 256 /**< maximum concurrent sessions */

/** 
 * A session is a series of operations performed by the same user. 
 * They may all occur during a single connection, but generally
 * a session can span multiple connections: it could be unbound from a
 * connection and then rebound later.
 */
typedef struct session {
   int index; /**< Sequence number. */
   string_t client_id;        /**< The 'mnemonic' identifier of the creator
                                   of this session. */
   connection_t *connection;  /**< Pointer to a connection that this session
                                   is bound to, or else NULL if it is
                                   currently idle. */
   struct msgqueue messages;  /**< Queue of pending messages for an idle
                                   session. */
   progress_t progress;       /**< Progress of time-consuming operations. */
   pthread_mutex_t mutex;     /**< Mutex for synchronizing job and connection
                                   threads. */
   pthread_t *job;            /**< Pointer to a variable containing of the
                                   ID of the thread performing the currently
                                   requested job. */
   int cancel;                /**< When set to 1, the job is being cancelled. */
   int cleanup;               /**< When set to 1, there is a cleanup in 
                                   progress. */
   pthread_mutex_t clnmutex;  /**< Mutex for synchronizing cancel variable. */
   pthread_cond_t jobnull;    /**< Condition variable to wait for a job to finish. */
   time_t last_used;          /**< When did this session last become idle? */
   struct sessopt options;    /**< Settable options for this session. */
   int qeflags;               /**< Extended flags for last successful query. */
   struct poliqarp_match_buffer *match_buffer;   /**< Match buffer. */
   struct poliqarp_corpus *corpus;               /**< Corpus. */
   bool closable_corpus;                         /**< Allow to close corpus? */
   struct poliqarp_query *query;                 /**< Query. */
   struct poliqarp_error error;                  /**< Last error. */
   struct poliqarp_random_state random_state; /**< State of the pseudo-random number generator. */
#ifdef HAVE_LOCALE_T
   locale_t locale; /**< Locale */
#endif
} session_t;

/**
 * This is an array of sessions, with holes and protected from accessing
 * by more than one thread at once. 
 */
struct session_list {
   pthread_mutex_t mutex;             /**< mutex for synchronization */
   session_t *sessions[MAX_SESSIONS]; /**< the actual array */
   int count;                         /**< number of sessions */
};

/** 
 * Initializes the session list. 
 */
void session_list_init(struct session_list *list);

/** 
 * Creates a new session (bound to a connection) with the given name 
 * in a list of sessions. Returns the index of a newly created session 
 * or an error value if the creation failed (see errors.h). 
 */
int session_insert(struct session_list *list, const char *client_id,
   connection_t *conn);

/** 
 * Deallocates all resources of the session. 
 */
void session_delete(session_t *session);

/** 
 * Deallocates all resources of an entry of the session list and removes
 * it from the list.
 */
void session_remove(struct session_list *list, int session);

/** 
 * Removes all sessions which have stayed idle (that is, not bound to any
 * connection) for more than max-session-idle seconds. 
 */
void session_remove_idle(struct session_list *list);

/** 
 * Marks an entry of a session list idle: that is, note that no connection
 * is currently using this session. 
 */
void session_idle(struct session_list *list, int session);

/** 
 * Removes all existing sesions from the list. 
 */
void session_cleanup(struct session_list *list);

/** 
 * Re-binds a connection to the session. 
 */
int session_rebind(struct session_list *list, int session, string_t uid, 
   connection_t *conn);

/**
 * Sends a message to a session. Works regardless of which state (idle 
 * or bound) the session is currently in. If the session is idle,
 * messages will be delivered as soon as somebody reconnects to it.
 * The message is freed as soon as it's delivered.
 */
void session_send(session_t *session, string_t msg);

/** 
 * Works exactly like session_send(), except that it sends a const char *
 * instead of string_t.
 */
void session_csend(session_t *session, const char *msg);

/** 
 * Launches a job for the session.
 */
void session_job_launch(session_t *session, void *(*routine)(void *), 
   void *data);

/**
 * Tries to cancel a running job.
 */
int session_job_cancel(session_t *session);

/** 
 * Performs job cleanup after a completed or interrupted job, setting the
 * session's job pointer to NULL. Must be called prior to outputting
 * any asynchronous return messages from this session.
 * @note session is really a pointer to session_t *. It is declared as 
 * void * to enable this function to be passed as an argument to 
 * pthread_cleanup_push(). Note also that any structure having a session
 * as its first member is also valid as an argument here.
 */
void session_job_cleanup(void *session);

#endif /* POLIQARPD_SESSION_H */
