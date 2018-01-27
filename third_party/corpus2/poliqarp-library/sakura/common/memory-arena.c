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

#include <poliqarp-config.h>

#include <sakura/common/memory-arena.h>

static inline size_t align_request(size_t request, size_t size)
{
   return ((request + size - 1) / size) * size;
}

struct marena_node *marena_alloc_node(size_t size)
{
   struct marena_node *this;

#if MARENA_DEBUG
   fprintf(stderr, "marena_alloc_node: %d\n", size);
#endif

   this = malloc(sizeof *this + size);
   if (this == NULL)
      return NULL; /* out of memory */
   this->next = NULL;
   this->used = 0;
   this->capacity = size;
   this->memory = ((char *)this) + sizeof *this;
   assert((((size_t)this->memory) & (MARENA_ALIGNMENT - 1)) == 0);
   return this;
}

struct marena_bignode *marena_alloc_bignode(size_t size)
{
   struct marena_bignode *this;

#if MARENA_DEBUG
   fprintf(stderr, "marena_alloc_bignode: %d\n", size);
#endif

   this = malloc(sizeof *this + size);
   if (this == NULL)
      return NULL; /* out of memory */
   this->next = NULL;
   this->size = size;
   this->memory = ((char *)this) + sizeof *this;
   assert((((size_t)this->memory) & (MARENA_ALIGNMENT - 1)) == 0);
   return this;
}

void *marena_get_node_memory(struct marena_node *this, size_t size)
{
   if (this->used + size <= this->capacity) {
      void *result = ((char *) this->memory) + this->used;
      this->used += size;
      return result;
   } else
      return NULL;
}

void marena_create(struct marena *this)
{
   this->first = NULL;
   this->current = NULL;
   this->first_big = NULL;
#if MARENA_TRACK_ALLOC
   this->n_alloc = 0;
#endif
   this->block_size = MARENA_DEF_BLOCK_SIZE;
}

void *marena_alloc(struct marena *this, size_t size)
{
   void *result;
   assert(this != NULL);

#if MARENA_DEBUG
   fprintf(stderr, "marena_alloc: %d -> %d\n", size, 
      align_request(size, MARENA_ALIGNMENT));
#endif

   /* align the request */
   size = align_request(size, MARENA_ALIGNMENT);

#if MARENA_TRACK_ALLOC
   this->n_alloc++;
#endif

   /* allocate large chunks separately */
   if (size > this->block_size) {
      struct marena_bignode *bignode = marena_alloc_bignode(size);
      if (bignode == NULL)
         return NULL; /* out of memory */
      bignode->next = this->first_big;
      this->first_big = bignode;
      return bignode->memory;
   }

   /* make sure we have a chain ready */
   if (this->current == NULL) {
      this->current = this->first = 
         marena_alloc_node(align_request(size, this->block_size));
      if (this->current == NULL)
         return NULL; /* out of memory */
   }

   /* try to allocate memory */
   result = marena_get_node_memory(this->current, size);
   if (result != NULL)
      return result;

   /* allocate another chain element */
   if (this->current->next == NULL) {
      this->current->next = 
         marena_alloc_node(align_request(size, this->block_size));
      if (this->current->next == NULL)
         return NULL; /* out of memory */
   }
   this->current = this->current->next;

   /* try to allocate memory again */
   result = marena_get_node_memory(this->current, size);
   assert(result != NULL);
   return result;
}

void marena_release(struct marena *this)
{
   struct marena_node *n = this->first;
   struct marena_bignode *bn, *bnext;
   for (bn = this->first_big; bn; bn = bnext) {
      bnext = bn->next;
      free(bn);
   }
   while (n) {
      n->used = 0;
      n = n->next;
   }
   this->current = this->first;
}

char *marena_strdup(struct marena *this, const char *string)
{
   size_t length;
   char *memory;

   length = strlen(string) + 1;
   memory = marena_alloc(this, length);
   if (memory == NULL)
      return NULL; /* out of memory */
   memcpy(memory, string, length);
   return memory;
}

char *marena_strndup(struct marena *this, const char *string, size_t length)
{
   char *memory;
   const char *end;

   end = memchr(string, 0, length);
   if (end)
      length = end - string;
   memory = marena_alloc(this, length + 1);
   if (memory == NULL)
      return NULL; /* out of memory */
   memcpy(memory, string, length);
   memory[length] = 0;
   return memory;
}

void marena_destroy(struct marena *this)
{
   struct marena_node *n, *next;
   struct marena_bignode *bn, *bnext;
   for (n = this->first; n; n = next) {
      next = n->next;
      free(n);
   }
   for (bn = this->first_big; bn; bn = bnext) {
      bnext = bn->next;
      free(bn);
   }
   this->first = NULL;
   this->current = NULL;
   this->first_big = NULL;
}

int marena_setopt(struct marena *this, int opt, size_t value)
{
   switch (opt) {
      case MARENA_OPT_BLOCKSIZE:
         this->block_size = value ? value : MARENA_DEF_BLOCK_SIZE;
         break;
      default:
         return -1;
   }
   return -1;
}

size_t marena_getopt(struct marena * this, int opt)
{
   switch (opt) {
   case MARENA_OPT_BLOCKSIZE:
      return this->block_size;
   case MARENA_OPT_NALLOC:
#if MARENA_TRACK_ALLOC
      return this->n_alloc;
#else
      return 0;
#endif
   default:
      return -1;
   }
   return -1;
}
