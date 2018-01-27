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

#ifndef HAVE_SET_H
#define HAVE_SET_H

#include <poliqarp-config.h>

#include <stdlib.h>
#include <stddef.h>

#include <sakura/common/memory-arena.h>

typedef int (*set_compare_fn)(const void *item1, const void *item2);

typedef void (*set_free_fn)(void *item);

struct set_node {
   struct set_node *next;
   void *item;
   int id;
};

struct set {
   struct set_node *first_node;
   int num_items;
   set_compare_fn item_compare;
   set_free_fn item_free;
   bool private_arena;
   struct marena *arena;
};

int set_create(struct set *this, set_compare_fn compare, set_free_fn free,
   struct marena *arena);

/** Destroy given set. */
void set_destroy(struct set *this);

/**
 * Put item in the set.
 * Duplicates are destroyed.
 */
struct set_node *set_put(struct set *this, void *item);

void *set_get(struct set *this, int id);

#endif
