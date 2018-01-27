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

#include "progress.h"

void progress_init(progress_t *progress)
{
   pthread_mutex_init(&progress->mutex, NULL);
   progress->progress = 0;
}

void progress_destroy(progress_t *progress)
{
   pthread_mutex_destroy(&progress->mutex);
}

void progress_set(progress_t *progress, int value)
{
   pthread_mutex_lock(&progress->mutex);
   progress->progress = value > 100 ? 100 : value < 0 ? 0 : value;
   pthread_mutex_unlock(&progress->mutex);
}

void progress_reset(progress_t *progress)
{
   progress_set(progress, 0);
}

void progress_advance(progress_t *progress, int amount)
{
   pthread_mutex_lock(&progress->mutex);
   progress->progress += amount;
   if (progress->progress > 100)
      progress->progress = 100;
   pthread_mutex_unlock(&progress->mutex);
}

int progress_get(progress_t *progress)
{
   int value;
   pthread_mutex_lock(&progress->mutex);
   value = progress->progress;
   pthread_mutex_unlock(&progress->mutex);
   return value;
}
