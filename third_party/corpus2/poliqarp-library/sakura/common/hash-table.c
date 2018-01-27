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

#include <string.h>

#include <sakura/common/hash-table.h>

int create_hash_table(struct hash_table *table, size_t size, int flags,
   struct marena *arena)
{
   table->size = size;
   table->num_items = 0;
   table->flags = flags;
   table->node_array = calloc(size, sizeof *table->node_array);
   if (table->node_array == NULL)
      return -1;
   if (arena) {
      table->arena = arena;
      table->private_arena = false;
   } else {
      table->arena = malloc(sizeof(*arena));
      if (table->arena == NULL) {
         free(table->node_array);
         return -1;
      }
      marena_create(table->arena);
      table->private_arena = true;
   }
   return 0;
}

int destroy_hash_table(struct hash_table *table, void (*free_fn)(void *))
{
   size_t i;
   struct hash_table_node *node, *next_node;
   if (free_fn) {
      for (i = 0; i < table->size; ++i) {
         for (node = table->node_array[i]; node; node = next_node) {
            next_node = node->next;
            free_fn(node->value);
         }
      }
   }
   if (table->private_arena) {
      marena_destroy(table->arena);
      free(table->arena);
   }
   free(table->node_array);
   return 0;
}

int hash_table_set(struct hash_table *table, const char *key, void *value)
{
   string_hash_t hash;
   struct hash_table_node *node;
   size_t index;

   hash = hash_string(key);
   index = hash % table->size;
   node = table->node_array[index];
   for (; node != NULL; node = node->next)
      if (hash == node->hash && strcmp(key, node->key) == 0) {
         node->value = value;
         return 0;
      }
   node = marena_alloc(table->arena, sizeof *node);
   if (!node)
      return -1;
   node->key = (table->flags & HASHTABLE_DUPLICATE_KEYS) ? 
      marena_strdup(table->arena, key) : (char *)key;
   if (node->key == NULL)
      return -1;
   node->value = value;
   node->hash = hash;
   node->next = table->node_array[index];
   table->node_array[index] = node;
   table->num_items++;
   return 0;
}

int hash_table_unset(struct hash_table *table, const char *key)
{
   string_hash_t hash;
   struct hash_table_node *node, *prev;
   size_t index;

   hash = hash_string(key);
   index = hash % table->size;
   node = table->node_array[index];
   prev = NULL;
   for (; node != NULL; prev = node, node = node->next)
      if (hash == node->hash && strcmp(key, node->key) == 0) {
         if (prev == NULL)
            table->node_array[index] = node->next;
         else
            prev->next = node->next;
         free(node->value);
         table->num_items--;
         return 0;
      }
   return -1;
 }

void *hash_table_get(struct hash_table *table, const char *key)
{
   string_hash_t hash;
   struct hash_table_node *node;
   hash = hash_string(key);
   node = table->node_array[hash % table->size];
   for (; node != NULL; node = node->next)
      if (hash == node->hash && strcmp(key, node->key) == 0)
         return node->value;
   return NULL;
}

const void *hash_table_const_get(const struct hash_table *table, 
   const char *key)
{
   return hash_table_get((struct hash_table *) table, key);
}

void hash_table_iterate(const struct hash_table *table, void *env,
   void (*iterator)(const char *, const void *, void *))
{
   size_t i;
   struct hash_table_node *node;
   for (i = 0; i < table->size; i++) {
      for (node = table->node_array[i]; node != NULL; node = node->next) {
         iterator(node->key, node->value, env);
      }
   }
}

size_t hash_table_num_items(const struct hash_table *table)
{
   return table->num_items;
}
