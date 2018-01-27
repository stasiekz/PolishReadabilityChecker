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

#include <sakura/common/entity.h>
#include <errno.h>

int create_entity_bag(struct entity_bag *this, struct marena *arena)
{
   assert(this != NULL);
   this->first_entity = NULL;
   this->num_items = 0;
   if (arena) {
      this->local_arena = false;
      this->arena = arena;
   } else {
      this->local_arena = true;
      this->arena = malloc(sizeof *this->arena);
      if (this->arena == NULL)
         return -1;
      marena_create(this->arena);
   }
   return 0;
}

void destroy_entity_bag(struct entity_bag *this)
{
   assert(this != NULL);
   if (this->arena != NULL && this->local_arena) {
      marena_destroy(this->arena);
      free(this->arena);
   }
}

/* entity construction */
struct entity *new_entity(struct entity_bag *this, const char *name, void *tag,
   void *data)
{
   struct entity *last = NULL;
   struct entity *current;
   struct entity *result;
   int cmp_result;

   assert(this != NULL);
   for (current = this->first_entity; current != NULL; 
        current = current->next_entity) 
   {
      cmp_result = strcmp(current->name, name);
      if (cmp_result == 0) {
         errno = EEXIST;
         return NULL;
      }
      if (cmp_result > 0)
         break;       /* keep ordering */
      last = current;
   }

   /* create new entity and link it with the rest */
   result = marena_alloc(this->arena, sizeof *result);
   if (result == NULL)
      return NULL;

   result->next_entity = current; 
   /* either NULL or last->next_entity so it's OK */
   
   result->name = marena_strdup(this->arena, name);
   if (result->name == NULL)
      return NULL;
   result->tag = tag;
   result->data = data;

   if (last)
      last->next_entity = result;
   else
      this->first_entity = result;

   /* count me */
   ++this->num_items;

   return result;
}

/* entity lookup */
struct entity *lookup_entity(struct entity_bag *this, const char *name)
{
   struct entity *entity;
   int cmp_result;

   assert(this != NULL);
   for (entity = this->first_entity; entity != NULL;
        entity = entity->next_entity) 
   {
      cmp_result = strcmp(entity->name, name);
      if (cmp_result > 0) {
         /* ordering failed, what we're looking for isn't here */
         errno = ENOENT;
         return NULL;
      }
      if (cmp_result != 0)
         continue;
      /* unalias */
      while (entity && entity->tag == entity)
         entity = entity->data;
      return entity;
   }
   errno = ENOENT;
   return NULL;
}

/* entity lookup */
const struct entity *lookup_const_entity(const struct entity_bag *this,
   const char *name)
{
   return lookup_entity((struct entity_bag *)this, name);
}

/* entity aliasing */
struct entity *new_entity_alias(struct entity_bag *this, struct entity *entity,
   const char *name)
{
   struct entity *result;
   result = new_entity(this, name, NULL, entity);
   if (result)
      result->tag = result;
   return result;
}
