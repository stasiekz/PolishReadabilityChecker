/*
 * This file is part of the Poliqarp suite.
 * 
 * Copyright (C) 2004-2010 by Instytut Podstaw Informatyki Polskiej
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
 * @file progress.h
 * @brief This is libprogress, the public library which is used by 
 * implementations of libpoliqarp to signal the progress of time-consuming 
 * calls. This library is reentrant and thread-safe.
 */

#ifndef PROGRESS_H
#define PROGRESS_H

#include <pthread.h>

/**
 * The type of a progress. It is an integer in the range 0-100, corresponding
 * to the percentage of already completed task (0 means not started yet,
 * 100 means complete.) 
 */
typedef struct {
   int progress;
   pthread_mutex_t mutex;
} progress_t;

/** Initializes the progress. Must be called before any other function. */
void progress_init(progress_t *progress);

/** Frees resources associated with the progress.
 *  No other function can be called after this one. */
void progress_destroy(progress_t *progress);

/** Re-sets the progress to zero. */
void progress_reset(progress_t *progress);

/** Sets the progress to the given value. */
void progress_set(progress_t *progress, int value);

/** Increases the progress by a given amount. */
void progress_advance(progress_t *progress, int amount);

/** Returns the value of the progress as an integer. */
int progress_get(progress_t *progress);

#endif /* PROGRESS_H */
