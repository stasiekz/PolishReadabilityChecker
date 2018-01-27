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

#include <sakura/common/set.h>

int set_create(struct set *this, set_compare_fn compare, set_free_fn free,
   struct marena *arena)
{
   this->first_node = NULL;
   this->num_items = 0;
   this->item_compare = compare;
   this->item_free = free;
   if (arena) {
      this->private_arena = false;
      this->arena = arena;
   } else {
      this->arena = malloc(sizeof *this->arena);
      if (this->arena == NULL)
         return -1;
      marena_create(this->arena);
   }
   return 0;
}

void set_destroy(struct set *this)
{
   struct set_node *curr;
   for (curr = this->first_node; curr; curr = curr->next)
      this->item_free(curr->item);
   if (this->private_arena) {
      marena_destroy(this->arena);
      free(this->arena);
   }
}

struct set_node *set_put(struct set *this, void *item)
{
   int cmp_result;
   struct set_node *current;
   struct set_node *last;
   struct set_node *new;

   for (last = NULL, current = this->first_node; current != NULL;
        current = current->next) 
   {
      cmp_result = this->item_compare(current->item, item);
      if (cmp_result > 0)        /* new element would break ordering */
         break;
      if (cmp_result == 0) {     /* identical element */
         this->item_free(item);
         return current;
      }
      last = current;
   }
   new = marena_alloc(this->arena, sizeof *new);
   if (new == NULL)
      return NULL;
   new->next = current;
   new->item = item;
   new->id = this->num_items++;

   if (last)
      last->next = new;
   else
      this->first_node = new;
   return new;
}

void *set_get(struct set *this, int id)
{
   struct set_node *current;

   for (current = this->first_node; current != NULL; current = current->next) {
      if (current->id > id)
         return NULL;
      if (current->id == id)  /* identical element */
         return current->item;
   }
   return NULL;
}
