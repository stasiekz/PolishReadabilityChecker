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
 * FIFO queues of strings. 
 */

#ifndef POLIQARPD_MSGQUEUE_H
#define POLIQARPD_MSGQUEUE_H

#include <foostring/foostring.h>

/**
 * A FIFO queue of strings (implemented as a singly-linked list).
 */
struct msgqueue {
   struct msgqueue_elem *first, *last; 
};

/**
 * An element of the queue.
 */
struct msgqueue_elem {
   string_t msg;
   struct msgqueue_elem *next;
};

/** 
 * Initializes the queue. 
 */
void msgqueue_init(struct msgqueue *queue);

/**
 * Inserts a string onto the end of the queue. 
 */
void msgqueue_append(struct msgqueue *queue, string_t str);

/**
 * Returns the first element in the queue, removing it. Returns NULL
 * if the queue was empty.
 */
string_t msgqueue_get(struct msgqueue *queue);

#endif /* POLIQARPD_MSGQUEUE_H */
